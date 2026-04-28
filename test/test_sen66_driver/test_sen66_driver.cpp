#include <unity.h>

#include "ArduinoMock.h"
#include "I2cMock.h"
#include "core/BootState.h"
#include "core/Logger.h"
#include "esp_system.h"

#define private public
#define Sen66 RealSen66
#include "../../src/drivers/Sen66.h"
#undef private

#include "../../src/core/I2CHelper.cpp"
#include "../../src/drivers/Sen66.cpp"
#undef Sen66

namespace {

void encodeWords(const uint16_t *words, size_t word_count, uint8_t *out) {
    for (size_t i = 0; i < word_count; ++i) {
        out[i * 3] = static_cast<uint8_t>(words[i] >> 8);
        out[i * 3 + 1] = static_cast<uint8_t>(words[i] & 0xFF);
        out[i * 3 + 2] = I2C::crc8(&out[i * 3], 2);
    }
}

} // namespace

void setUp() {
    setMillis(0);
    I2cMock::reset();
    Logger::begin(Serial, Logger::Debug);
    Logger::setSerialOutputEnabled(false);
    Logger::setSensorsSerialOutputEnabled(false);
    boot_reset_reason = ESP_RST_POWERON;
}

void tearDown() {}

void test_real_sen66_device_reset_clears_co2_smoother_ring() {
    I2cMock::setDevicePresent(Config::SEN66_ADDR, true);

    RealSen66 sen66;
    TEST_ASSERT_TRUE(sen66.begin());

    sen66.co2_first_ = false;
    sen66.co2_idx_ = 3;
    sen66.co2_readings_[0] = 1111;
    sen66.co2_readings_[1] = 1222;
    sen66.co2_readings_[2] = 1333;
    sen66.co2_readings_[3] = 1444;
    sen66.co2_readings_[4] = 1555;

    TEST_ASSERT_TRUE(sen66.deviceReset());
    TEST_ASSERT_TRUE(sen66.co2_first_);
    TEST_ASSERT_EQUAL(0, sen66.co2_idx_);
    for (int reading : sen66.co2_readings_) {
        TEST_ASSERT_EQUAL(400, reading);
    }
}

void test_real_sen66_apply_temp_offset_params_includes_base_self_heating() {
    I2cMock::setDevicePresent(Config::SEN66_ADDR, true);

    RealSen66 sen66;
    TEST_ASSERT_TRUE(sen66.begin());

    sen66.temp_offset_ = 1.5f;

    const float expected_hw_correction = 1.5f - Config::BASE_TEMP_OFFSET;
    TEST_ASSERT_FLOAT_WITHIN(0.001f, expected_hw_correction, sen66.desiredTempCorrectionC());
    TEST_ASSERT_TRUE(sen66.applyTempOffsetParams());
    TEST_ASSERT_TRUE(sen66.temp_offset_hw_active_);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, expected_hw_correction, sen66.temp_offset_hw_value_);
}

void test_real_sen66_read_values_applies_remaining_temp_correction_when_hw_offset_is_stale() {
    I2cMock::setDevicePresent(Config::SEN66_ADDR, true);

    const uint16_t read_values_words[9] = {
        0,
        0,
        0,
        0,
        5000,
        4800,
        0x7FFF,
        0x7FFF,
        500
    };
    uint8_t read_values_buf[27] = {};
    encodeWords(read_values_words, 9, read_values_buf);
    I2cMock::setCommandRead(Config::SEN66_ADDR,
                            Config::SEN66_CMD_READ_VALUES,
                            read_values_buf,
                            sizeof(read_values_buf));

    const uint16_t num_conc_words[5] = {
        0xFFFF,
        0,
        0,
        0,
        0
    };
    uint8_t num_conc_buf[15] = {};
    encodeWords(num_conc_words, 5, num_conc_buf);
    I2cMock::setCommandRead(Config::SEN66_ADDR,
                            Config::SEN66_CMD_READ_NUM_CONC,
                            num_conc_buf,
                            sizeof(num_conc_buf));

    RealSen66 sen66;
    TEST_ASSERT_TRUE(sen66.begin());
    sen66.temp_offset_ = 0.0f;
    sen66.temp_offset_hw_active_ = true;
    sen66.temp_offset_hw_value_ = -1.0f;

    SensorData data{};
    TEST_ASSERT_TRUE(sen66.readValues(data));

    TEST_ASSERT_TRUE(data.temp_valid);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 22.6f, data.temperature);
    TEST_ASSERT_TRUE(data.hum_valid);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 50.0f, data.humidity);
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_real_sen66_device_reset_clears_co2_smoother_ring);
    RUN_TEST(test_real_sen66_apply_temp_offset_params_includes_base_self_heating);
    RUN_TEST(test_real_sen66_read_values_applies_remaining_temp_correction_when_hw_offset_is_stale);
    return UNITY_END();
}

