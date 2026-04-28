#include <unity.h>

#include "ArduinoMock.h"
#include "SntpMock.h"
#include "TimeMock.h"
#include "core/BootState.h"
#include "modules/StorageManager.h"
#include "modules/TimeManager.h"

void setUp() {
    setMillis(0);
    setNowEpoch(0);
    boot_reset_reason = ESP_RST_POWERON;
    SntpMock::reset();
}

void tearDown() {}

void test_time_manager_ntp_uses_default_public_servers_when_custom_server_is_empty() {
    StorageManager storage;
    storage.begin();

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_TRUE(manager.updateWifiState(true, true));
    TEST_ASSERT_TRUE(SntpMock::wasConfigCalled());
    TEST_ASSERT_TRUE(SntpMock::isEnabled());
    TEST_ASSERT_EQUAL_STRING("pool.ntp.org", SntpMock::lastServer1());
    TEST_ASSERT_EQUAL_STRING("time.nist.gov", SntpMock::lastServer2());
    TEST_ASSERT_EQUAL_STRING("time.google.com", SntpMock::lastServer3());
}

void test_time_manager_ntp_uses_custom_server_when_configured() {
    StorageManager storage;
    storage.begin();
    storage.config().ntp_server = "192.168.1.1";

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_TRUE(manager.updateWifiState(true, true));
    TEST_ASSERT_TRUE(SntpMock::wasConfigCalled());
    TEST_ASSERT_EQUAL_STRING("192.168.1.1", SntpMock::lastServer1());
    TEST_ASSERT_EQUAL_STRING("", SntpMock::lastServer2());
    TEST_ASSERT_EQUAL_STRING("", SntpMock::lastServer3());
}

void test_time_manager_set_ntp_server_pref_restarts_sync_with_new_server() {
    StorageManager storage;
    storage.begin();

    TimeManager manager;
    manager.begin(storage);
    TEST_ASSERT_TRUE(manager.updateWifiState(true, true));
    TEST_ASSERT_EQUAL_STRING("pool.ntp.org", SntpMock::lastServer1());

    advanceMillis(100);
    TEST_ASSERT_TRUE(manager.setNtpServerPref(" router.local "));
    TEST_ASSERT_EQUAL_STRING("router.local", manager.ntpServerPref().c_str());
    TEST_ASSERT_EQUAL_STRING("router.local", SntpMock::lastServer1());
    TEST_ASSERT_EQUAL_STRING("", SntpMock::lastServer2());
    TEST_ASSERT_EQUAL_STRING("", SntpMock::lastServer3());
}

void test_time_manager_prefers_timezone_name_over_legacy_index() {
    StorageManager storage;
    storage.begin();
    storage.config().tz_name = "Australia/Sydney";
    storage.config().tz_index = TimeManager::findTimezoneIndex("Europe/London");

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_EQUAL_STRING("Australia/Sydney", manager.getTimezone().name);
    TEST_ASSERT_EQUAL_INT(TimeManager::findTimezoneIndex("Australia/Sydney"),
                          storage.config().tz_index);
    TEST_ASSERT_EQUAL_STRING("Australia/Sydney", storage.config().tz_name.c_str());

    TEST_ASSERT_TRUE(manager.updateWifiState(true, true));
    TEST_ASSERT_EQUAL_STRING("AEST-10AEDT,M10.1.0,M4.1.0/3", SntpMock::lastTimezone());
}

void test_time_manager_migrates_legacy_timezone_index_to_name() {
    StorageManager storage;
    storage.begin();
    storage.config().tz_index = 14;  // Europe/Paris in the pre-Berlin numeric list.

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_EQUAL_STRING("Europe/Paris", manager.getTimezone().name);
    TEST_ASSERT_EQUAL_INT(TimeManager::findTimezoneIndex("Europe/Paris"), storage.config().tz_index);
    TEST_ASSERT_EQUAL_STRING("Europe/Paris", storage.config().tz_name.c_str());
}

void test_time_manager_set_timezone_index_persists_name_and_index() {
    StorageManager storage;
    storage.begin();

    TimeManager manager;
    manager.begin(storage);

    const int new_york_index = TimeManager::findTimezoneIndex("America/New_York");
    TEST_ASSERT_TRUE(manager.setTimezoneIndex(new_york_index));
    TEST_ASSERT_EQUAL_STRING("America/New_York", storage.config().tz_name.c_str());
    TEST_ASSERT_EQUAL_INT(new_york_index, storage.config().tz_index);
}

void test_time_manager_supports_brisbane_without_dst() {
    StorageManager storage;
    storage.begin();
    storage.config().tz_name = "Australia/Brisbane";

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_EQUAL_STRING("Australia/Brisbane", manager.getTimezone().name);
    TEST_ASSERT_EQUAL_INT(10 * 60, manager.getTimezone().offset_min);
    TEST_ASSERT_EQUAL_STRING("Australia/Brisbane", storage.config().tz_name.c_str());

    TEST_ASSERT_TRUE(manager.updateWifiState(true, true));
    TEST_ASSERT_EQUAL_STRING("AEST-10", SntpMock::lastTimezone());
}

void test_time_manager_supports_berlin_with_dst_rules() {
    StorageManager storage;
    storage.begin();
    storage.config().tz_name = "Europe/Berlin";

    TimeManager manager;
    manager.begin(storage);

    TEST_ASSERT_EQUAL_STRING("Europe/Berlin", manager.getTimezone().name);
    TEST_ASSERT_EQUAL_INT(60, manager.getTimezone().offset_min);
    TEST_ASSERT_EQUAL_STRING("Europe/Berlin", storage.config().tz_name.c_str());

    TEST_ASSERT_TRUE(manager.updateWifiState(true, true));
    TEST_ASSERT_EQUAL_STRING("CET-1CEST,M3.5.0,M10.5.0/3", SntpMock::lastTimezone());
}

void test_time_manager_berlin_reports_current_dst_offset() {
    StorageManager storage;
    storage.begin();
    storage.config().tz_name = "Europe/Berlin";

    TimeManager manager;
    manager.begin(storage);

    setNowEpoch(1768478400);  // 2026-01-15 12:00:00 UTC
    TEST_ASSERT_EQUAL_INT(60, manager.currentUtcOffsetMinutes());

    setNowEpoch(1777377600);  // 2026-04-28 12:00:00 UTC
    TEST_ASSERT_EQUAL_INT(120, manager.currentUtcOffsetMinutes());
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_time_manager_ntp_uses_default_public_servers_when_custom_server_is_empty);
    RUN_TEST(test_time_manager_ntp_uses_custom_server_when_configured);
    RUN_TEST(test_time_manager_set_ntp_server_pref_restarts_sync_with_new_server);
    RUN_TEST(test_time_manager_prefers_timezone_name_over_legacy_index);
    RUN_TEST(test_time_manager_migrates_legacy_timezone_index_to_name);
    RUN_TEST(test_time_manager_set_timezone_index_persists_name_and_index);
    RUN_TEST(test_time_manager_supports_brisbane_without_dst);
    RUN_TEST(test_time_manager_supports_berlin_with_dst_rules);
    RUN_TEST(test_time_manager_berlin_reports_current_dst_offset);
    return UNITY_END();
}
