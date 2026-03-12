#include <unity.h>

#include "ArduinoMock.h"
#include "I2cMock.h"
#include "SntpMock.h"
#include "core/Logger.h"
#include "modules/StorageManager.h"
#include "modules/TimeManager.h"

namespace {

uint8_t toBcd(uint8_t value) {
    return static_cast<uint8_t>(value + 6 * (value / 10));
}

void seedPcf8523WithOldValidTime() {
    I2cMock::setDevicePresent(Config::PCF8523_ADDR, true);

    const uint8_t signature[] = {0x00, 0x00, 0x07, 0x00, 0x07};
    I2cMock::setRegisters(Config::PCF8523_ADDR, Config::PCF8523_REG_OFFSET,
                          signature, sizeof(signature));

    const uint8_t time_regs[] = {
        toBcd(56), toBcd(34), toBcd(12), toBcd(15), 0x02, toBcd(4), toBcd(19)
    };
    I2cMock::setRegisters(Config::PCF8523_ADDR, Config::PCF8523_REG_SECONDS,
                          time_regs, sizeof(time_regs));
    I2cMock::setRegister(Config::PCF8523_ADDR, Config::PCF8523_REG_CONTROL_3, 0x00);
}

void seedDs3231WithOldValidTime() {
    I2cMock::setDevicePresent(Config::DS3231_ADDR, true);

    const uint8_t meta_regs[] = {0x00, 0x00, 0x19, 0x40};
    I2cMock::setRegisters(Config::DS3231_ADDR, Config::DS3231_REG_STATUS,
                          meta_regs, sizeof(meta_regs));

    const uint8_t time_regs[] = {
        toBcd(56), toBcd(34), toBcd(12), 0x02, toBcd(15), toBcd(4), toBcd(19)
    };
    I2cMock::setRegisters(Config::DS3231_ADDR, Config::DS3231_REG_SECONDS,
                          time_regs, sizeof(time_regs));
}

void seedDs3231ThatLooksLikePcf8523Fallback() {
    I2cMock::setDevicePresent(Config::DS3231_ADDR, true);

    const uint8_t meta_regs[] = {0x00, 0x00, 0x19, 0x40};
    I2cMock::setRegisters(Config::DS3231_ADDR, Config::DS3231_REG_STATUS,
                          meta_regs, sizeof(meta_regs));

    const uint8_t time_regs[] = {
        toBcd(8), toBcd(34), toBcd(9), 4, toBcd(12), toBcd(3), toBcd(19)
    };
    I2cMock::setRegisters(Config::DS3231_ADDR, Config::DS3231_REG_SECONDS,
                          time_regs, sizeof(time_regs));

    const uint8_t alarm1_regs[] = {3, 4, 5};
    I2cMock::setRegisters(Config::DS3231_ADDR, 0x07, alarm1_regs, sizeof(alarm1_regs));
}

} // namespace

void setUp() {
    setMillis(0);
    I2cMock::reset();
    SntpMock::reset();
    Logger::begin(Serial, Logger::Debug);
    Logger::setSerialOutputEnabled(false);
    Logger::setSensorsSerialOutputEnabled(false);
    Logger::resetRecentForTest();
}

void tearDown() {
    Logger::resetRecentForTest();
}

void test_time_manager_init_rtc_handles_absent_rtc() {
    StorageManager storage;
    storage.begin();

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_FALSE(manager.initRtc());
    TEST_ASSERT_FALSE(manager.isRtcPresent());
    TEST_ASSERT_FALSE(manager.isRtcValid());
    TEST_ASSERT_FALSE(manager.isRtcLostPower());
    TEST_ASSERT_EQUAL_STRING("RTC", manager.rtcLabel());
}

void test_time_manager_init_rtc_selects_pcf8523() {
    seedPcf8523WithOldValidTime();

    StorageManager storage;
    storage.begin();

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_FALSE(manager.initRtc());
    TEST_ASSERT_TRUE(manager.isRtcPresent());
    TEST_ASSERT_FALSE(manager.isRtcValid());
    TEST_ASSERT_FALSE(manager.isRtcLostPower());
    TEST_ASSERT_EQUAL_STRING("PCF8523", manager.rtcLabel());
}

void test_time_manager_init_rtc_selects_ds3231() {
    seedDs3231WithOldValidTime();

    StorageManager storage;
    storage.begin();

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_FALSE(manager.initRtc());
    TEST_ASSERT_TRUE(manager.isRtcPresent());
    TEST_ASSERT_FALSE(manager.isRtcValid());
    TEST_ASSERT_FALSE(manager.isRtcLostPower());
    TEST_ASSERT_EQUAL_STRING("DS3231", manager.rtcLabel());
}

void test_time_manager_init_rtc_prefers_ds3231_before_pcf8523_fallback() {
    seedDs3231ThatLooksLikePcf8523Fallback();

    StorageManager storage;
    storage.begin();

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_FALSE(manager.initRtc());
    TEST_ASSERT_TRUE(manager.isRtcPresent());
    TEST_ASSERT_FALSE(manager.isRtcValid());
    TEST_ASSERT_FALSE(manager.isRtcLostPower());
    TEST_ASSERT_EQUAL_STRING("DS3231", manager.rtcLabel());
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_time_manager_init_rtc_handles_absent_rtc);
    RUN_TEST(test_time_manager_init_rtc_selects_pcf8523);
    RUN_TEST(test_time_manager_init_rtc_selects_ds3231);
    RUN_TEST(test_time_manager_init_rtc_prefers_ds3231_before_pcf8523_fallback);
    return UNITY_END();
}
