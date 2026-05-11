#include <unity.h>

#include <ArduinoJson.h>

#include "modules/DisplayThresholds.h"

void setUp() {}
void tearDown() {}

void test_display_threshold_defaults_classify_current_visual_bands() {
    const DisplayThresholds::Config cfg = DisplayThresholds::defaults();

    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Green),
                          static_cast<int>(DisplayThresholds::classifyRange(22.0f, cfg.temp)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Yellow),
                          static_cast<int>(DisplayThresholds::classifyRange(18.5f, cfg.temp)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Orange),
                          static_cast<int>(DisplayThresholds::classifyRange(16.5f, cfg.temp)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Red),
                          static_cast<int>(DisplayThresholds::classifyRange(15.5f, cfg.temp)));

    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Green),
                          static_cast<int>(DisplayThresholds::classifyRange(12.0f, cfg.dew_point)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Green),
                          static_cast<int>(DisplayThresholds::classifyRange(10.0f, cfg.ah)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Green),
                          static_cast<int>(DisplayThresholds::classifyHigh(700.0f, cfg.co2)));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(DisplayThresholds::Band::Red),
                          static_cast<int>(DisplayThresholds::classifyHigh(101.0f, cfg.hcho)));
}

void test_display_threshold_validation_rejects_bad_order() {
    DisplayThresholds::Config cfg = DisplayThresholds::defaults();
    String error;

    cfg.co2.green = 1200.0f;
    TEST_ASSERT_FALSE(DisplayThresholds::validate(cfg, &error));
    TEST_ASSERT_TRUE(error.length() > 0);

    cfg = DisplayThresholds::defaults();
    cfg.rh.good_min = cfg.rh.good_max;
    TEST_ASSERT_FALSE(DisplayThresholds::validate(cfg, &error));
    TEST_ASSERT_TRUE(error.length() > 0);
}

void test_display_threshold_serialize_roundtrip_preserves_values_and_switches() {
    DisplayThresholds::Config cfg = DisplayThresholds::defaults();
    cfg.co.orange = 77.0f;
    cfg.temp.good_min = 19.0f;
    cfg.background_alerts.hcho_enabled = false;
    cfg.background_alerts.co2_enabled = false;

    const String json = DisplayThresholds::serialize(cfg);
    DisplayThresholds::Config parsed{};
    TEST_ASSERT_TRUE(DisplayThresholds::deserialize(json, parsed));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 77.0f, parsed.co.orange);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 19.0f, parsed.temp.good_min);
    TEST_ASSERT_FALSE(parsed.background_alerts.hcho_enabled);
    TEST_ASSERT_FALSE(parsed.background_alerts.co2_enabled);
    TEST_ASSERT_TRUE(parsed.background_alerts.co_enabled);
}

void test_display_threshold_partial_update_keeps_unspecified_values() {
    DisplayThresholds::Config current = DisplayThresholds::defaults();
    ArduinoJson::JsonDocument doc;
    deserializeJson(doc,
                    "{\"metrics\":{\"co2\":{\"green\":700,\"yellow\":900,\"orange\":1400}},"
                    "\"background_alerts\":{\"co_enabled\":false}}");

    DisplayThresholds::Config updated{};
    TEST_ASSERT_TRUE(DisplayThresholds::applyUpdateJson(doc.as<ArduinoJson::JsonVariantConst>(),
                                                        current,
                                                        updated));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 700.0f, updated.co2.green);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, current.hcho.orange, updated.hcho.orange);
    TEST_ASSERT_FALSE(updated.background_alerts.co_enabled);
    TEST_ASSERT_TRUE(updated.background_alerts.hcho_enabled);
}

void test_display_threshold_manager_falls_back_to_defaults_and_persists_reset() {
    StorageManager storage;
    storage.begin();
    TEST_ASSERT_TRUE(storage.saveTextAtomic(StorageManager::kDisplayThresholdsPath, "{bad"));

    DisplayThresholdManager manager;
    manager.begin(storage);
    DisplayThresholds::Config cfg = manager.snapshot();
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 800.0f, cfg.co2.green);

    cfg.co2.green = 700.0f;
    cfg.co2.yellow = 900.0f;
    cfg.co2.orange = 1400.0f;
    TEST_ASSERT_TRUE(manager.apply(cfg));

    DisplayThresholdManager loaded;
    loaded.begin(storage);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 700.0f, loaded.snapshot().co2.green);

    TEST_ASSERT_TRUE(loaded.reset());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 800.0f, loaded.snapshot().co2.green);
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_display_threshold_defaults_classify_current_visual_bands);
    RUN_TEST(test_display_threshold_validation_rejects_bad_order);
    RUN_TEST(test_display_threshold_serialize_roundtrip_preserves_values_and_switches);
    RUN_TEST(test_display_threshold_partial_update_keeps_unspecified_values);
    RUN_TEST(test_display_threshold_manager_falls_back_to_defaults_and_persists_reset);
    return UNITY_END();
}
