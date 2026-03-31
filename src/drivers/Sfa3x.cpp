// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "Sfa3x.h"
#include <math.h>
#include "core/BootState.h"
#include "core/Logger.h"
#include "config/AppConfig.h"
#include "core/I2CHelper.h"

namespace {

bool sfa3xStateUnknownAfterBoot() {
    return boot_reset_reason != ESP_RST_POWERON;
}

} // namespace

bool Sfa3x::begin() {
    ok_ = true;
    measuring_ = false;
    measurement_state_unknown_ = sfa3xStateUnknownAfterBoot();
    data_valid_ = false;
    has_new_data_ = false;
    last_hcho_ppb_ = 0.0f;
    last_poll_ms_ = 0;
    last_data_ms_ = 0;
    warmup_started_ms_ = 0;
    fail_count_ = 0;
    variant_ = Variant::Unknown;
    status_ = Status::Absent;
    last_error_cause_ = ErrorCause::None;
    warmup_active_ = false;
    return true;
}

void Sfa3x::start() {
    if (measuring_) {
        return;
    }
    if (!pingAddress()) {
        ok_ = false;
        status_ = Status::Absent;
        variant_ = Variant::Unknown;
        last_error_cause_ = ErrorCause::None;
        warmup_active_ = false;
        warmup_started_ms_ = 0;
        return;
    }

    if (!ensureIdleBeforeDetect()) {
        ok_ = false;
        status_ = Status::Fault;
        LOGW("SFA3X", "variant pre-stop failed (%s)", errorCauseLabel());
        return;
    }

    detectVariant();

    // The device ACKed on the bus, so any STOP/START failure is a fault, not absence.
    status_ = Status::Fault;
    if (!ensureIdleBeforeStart()) {
        ok_ = false;
        LOGW(label(), "start aborted (%s)", errorCauseLabel());
        return;
    }
    const uint16_t start_cmd =
        (variant_ == Variant::Sfa40) ? Config::SFA40_CMD_START : Config::SFA3X_CMD_START;
    if (!writeCmd(start_cmd)) {
        ok_ = false;
        last_error_cause_ = ErrorCause::StartCommand;
        LOGW(label(), "start failed (%s)", errorCauseLabel());
        return;
    }
    delay(Config::SFA3X_START_DELAY_MS);
    measuring_ = true;
    measurement_state_unknown_ = false;
    warmup_started_ms_ = millis();
    warmup_active_ = (variant_ == Variant::Sfa40);
    ok_ = true;
    status_ = Status::Ok;
    last_error_cause_ = ErrorCause::None;
}

void Sfa3x::stop() {
    if (!measuring_ && !measurement_state_unknown_) {
        return;
    }
    const uint16_t stop_cmd =
        (variant_ == Variant::Sfa40) ? Config::SFA40_CMD_STOP : Config::SFA3X_CMD_STOP;
    if (!writeCmd(stop_cmd)) {
        return;
    }
    delay(Config::SFA3X_STOP_DELAY_MS);
    measuring_ = false;
    measurement_state_unknown_ = false;
    warmup_active_ = false;
    warmup_started_ms_ = 0;
}

bool Sfa3x::readData(float &hcho_ppb) {
    if (variant_ == Variant::Sfa40) {
        uint16_t words[4];
        if (!readWords(Config::SFA40_CMD_READ_VALUES, words, 4, 0)) {
            return false;
        }
        if ((words[3] & 0x0001U) != 0U || (words[3] & 0x0002U) != 0U) {
            last_error_cause_ = ErrorCause::ReadStatus;
            refreshWarmupState(millis());
            return false;
        }
        warmup_active_ = false;
        hcho_ppb = static_cast<float>(words[0]) / 10.0f;
        return true;
    }

    uint16_t words[3];
    if (!readWords(Config::SFA3X_CMD_READ_VALUES, words, 3, Config::SFA3X_READ_DELAY_MS)) {
        return false;
    }
    const int16_t hcho_raw = static_cast<int16_t>(words[0]);
    hcho_ppb = hcho_raw / 5.0f;
    return true;
}

void Sfa3x::poll() {
    if (!ok_ || !measuring_) {
        return;
    }
    const uint32_t now = millis();
    refreshWarmupState(now);
    if (now - last_poll_ms_ < Config::SFA3X_POLL_MS) {
        return;
    }
    last_poll_ms_ = now;

    float hcho_ppb = 0.0f;
    if (!readData(hcho_ppb)) {
        if (variant_ == Variant::Sfa40 &&
            last_error_cause_ == ErrorCause::ReadStatus) {
            if (warmup_active_) {
                status_ = Status::Ok;
                return;
            }
            const bool was_fault = (status_ == Status::Fault);
            status_ = Status::Fault;
            fail_count_ = 0;
            if (!was_fault) {
                LOGW(label(), "read values failed (%s)", errorCauseLabel());
            }
            return;
        }
        if (++fail_count_ == 3) {
            if (status_ != Status::Absent) {
                status_ = Status::Fault;
                LOGW(label(), "read values failed (%s)", errorCauseLabel());
            }
            fail_count_ = 0;
        }
        return;
    }

    fail_count_ = 0;
    status_ = Status::Ok;
    last_error_cause_ = ErrorCause::None;
    if (isfinite(hcho_ppb) && hcho_ppb >= 0.0f) {
        last_hcho_ppb_ = hcho_ppb;
        data_valid_ = true;
        has_new_data_ = true;
        last_data_ms_ = now;
        warmup_active_ = false;
    }
}

bool Sfa3x::takeNewData(float &hcho_ppb) {
    if (!has_new_data_ || !data_valid_) {
        return false;
    }
    hcho_ppb = last_hcho_ppb_;
    has_new_data_ = false;
    return true;
}

void Sfa3x::invalidate() {
    data_valid_ = false;
    has_new_data_ = false;
}

bool Sfa3x::readWords(uint16_t cmd, uint16_t *out, size_t words, uint32_t delay_ms) {
    if (!writeCmd(cmd)) {
        last_error_cause_ = ErrorCause::ReadCommand;
        return false;
    }
    delay(delay_ms);
    const size_t bytes = words * 3;
    uint8_t buf[12];
    if (bytes > sizeof(buf)) {
        return false;
    }
    if (!readBytes(buf, bytes)) {
        last_error_cause_ = ErrorCause::ReadBytes;
        return false;
    }
    for (size_t i = 0; i < words; ++i) {
        const uint8_t *p = &buf[i * 3];
        if (I2C::crc8(p, 2) != p[2]) {
            last_error_cause_ = ErrorCause::ReadCrc;
            return false;
        }
        out[i] = (static_cast<uint16_t>(p[0]) << 8) | p[1];
    }
    return true;
}

void Sfa3x::detectVariant() {
    uint16_t words[3];
    if (readWords(Config::SFA40_CMD_ID, words, 3, 0)) {
        bool has_non_zero_serial = false;
        for (uint16_t word : words) {
            if (word != 0) {
                has_non_zero_serial = true;
                break;
            }
        }
        if (has_non_zero_serial) {
            variant_ = Variant::Sfa40;
            last_error_cause_ = ErrorCause::None;
            return;
        }
    }

    variant_ = Variant::Sfa30;
    last_error_cause_ = ErrorCause::None;
}

void Sfa3x::refreshWarmupState(uint32_t now_ms) {
    if (!warmup_active_ || variant_ != Variant::Sfa40) {
        return;
    }
    if (warmup_started_ms_ == 0 || (now_ms - warmup_started_ms_) >= Config::SFA40_WARMUP_MS) {
        warmup_active_ = false;
    }
}

bool Sfa3x::ensureIdleBeforeDetect() {
    if (!measurement_state_unknown_) {
        return true;
    }

    LOGI("SFA3X", "forcing idle before variant detect after warm restart");

    bool stopped = false;
    if (writeCmd(Config::SFA40_CMD_STOP)) {
        delay(Config::SFA3X_STOP_DELAY_MS);
        stopped = true;
    }
    if (writeCmd(Config::SFA3X_CMD_STOP)) {
        delay(Config::SFA3X_STOP_DELAY_MS);
        stopped = true;
    }
    if (!stopped) {
        last_error_cause_ = ErrorCause::WarmRestartStop;
        return false;
    }

    measuring_ = false;
    measurement_state_unknown_ = false;
    last_error_cause_ = ErrorCause::None;
    return true;
}

bool Sfa3x::pingAddress() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (!cmd) {
        return false;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (Config::SFA3X_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    const esp_err_t err = i2c_master_cmd_begin(
        Config::I2C_PORT,
        cmd,
        pdMS_TO_TICKS(Config::I2C_TIMEOUT_MS)
    );
    i2c_cmd_link_delete(cmd);
    return err == ESP_OK;
}

bool Sfa3x::writeCmd(uint16_t cmd) {
    return I2C::write_cmd(Config::SFA3X_ADDR, cmd, nullptr, 0) == ESP_OK;
}

bool Sfa3x::readBytes(uint8_t *buf, size_t len) {
    return I2C::read_bytes(Config::SFA3X_ADDR, buf, len) == ESP_OK;
}

bool Sfa3x::ensureIdleBeforeStart() {
    if (!measurement_state_unknown_) {
        return true;
    }

    LOGI(label(), "forcing idle after warm restart");
    const uint16_t stop_cmd =
        (variant_ == Variant::Sfa40) ? Config::SFA40_CMD_STOP : Config::SFA3X_CMD_STOP;
    if (!writeCmd(stop_cmd)) {
        last_error_cause_ = ErrorCause::WarmRestartStop;
        return false;
    }
    delay(Config::SFA3X_STOP_DELAY_MS);
    measuring_ = false;
    measurement_state_unknown_ = false;
    last_error_cause_ = ErrorCause::None;
    return true;
}

const char *Sfa3x::errorCauseLabel() const {
    switch (last_error_cause_) {
        case ErrorCause::WarmRestartStop:
            return "warm-restart-stop";
        case ErrorCause::StartCommand:
            return "start-cmd";
        case ErrorCause::ReadCommand:
            return "read-cmd";
        case ErrorCause::ReadBytes:
            return "read-bytes";
        case ErrorCause::ReadCrc:
            return "crc";
        case ErrorCause::ReadStatus:
            return "status";
        default:
            return "unknown";
    }
}

const char *Sfa3x::label() const {
    switch (variant_) {
        case Variant::Sfa40:
            return "SFA40";
        case Variant::Sfa30:
            return "SFA30";
        default:
            return "SFA3X";
    }
}
