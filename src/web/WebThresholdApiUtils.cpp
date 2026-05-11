// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "web/WebThresholdApiUtils.h"

namespace WebThresholdApiUtils {

void fillStateJson(ArduinoJson::JsonObject root,
                   const DisplayThresholds::Config &active,
                   const DisplayThresholds::Config &factory) {
    root["success"] = true;
    root["version"] = DisplayThresholds::kVersion;
    DisplayThresholds::writeMetricsJson(root["metrics"].to<ArduinoJson::JsonObject>(), active);
    DisplayThresholds::writeBackgroundAlertsJson(
        root["background_alerts"].to<ArduinoJson::JsonObject>(), active.background_alerts);
    DisplayThresholds::writeMetricsJson(root["factory_metrics"].to<ArduinoJson::JsonObject>(), factory);
    DisplayThresholds::writeBackgroundAlertsJson(
        root["factory_background_alerts"].to<ArduinoJson::JsonObject>(), factory.background_alerts);
}

void fillSuccessJson(ArduinoJson::JsonObject root,
                     const DisplayThresholds::Config &active) {
    root["success"] = true;
    root["version"] = DisplayThresholds::kVersion;
    DisplayThresholds::writeMetricsJson(root["metrics"].to<ArduinoJson::JsonObject>(), active);
    DisplayThresholds::writeBackgroundAlertsJson(
        root["background_alerts"].to<ArduinoJson::JsonObject>(), active.background_alerts);
}

void fillErrorJson(ArduinoJson::JsonObject root, const char *message) {
    root["success"] = false;
    root["error"] = message ? message : "Threshold update failed";
}

ParseResult parseUpdateRequestBody(const String &body,
                                   const DisplayThresholds::Config &current) {
    ParseResult result{};
    if (body.length() == 0) {
        result.error_message = "Missing body";
        return result;
    }

    ArduinoJson::JsonDocument doc;
    const ArduinoJson::DeserializationError err = ArduinoJson::deserializeJson(doc, body);
    if (err) {
        result.error_message = "Invalid JSON";
        return result;
    }

    String error;
    DisplayThresholds::Config updated{};
    if (!DisplayThresholds::applyUpdateJson(doc.as<ArduinoJson::JsonVariantConst>(),
                                            current,
                                            updated,
                                            &error)) {
        result.error_message = error.length() ? error : "Invalid thresholds";
        return result;
    }

    result.success = true;
    result.status_code = 200;
    result.update = updated;
    return result;
}

}  // namespace WebThresholdApiUtils
