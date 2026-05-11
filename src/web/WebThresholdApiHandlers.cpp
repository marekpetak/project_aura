// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "web/WebThresholdApiHandlers.h"

#include <ArduinoJson.h>

#include "web/WebResponseUtils.h"
#include "web/WebThresholdApiUtils.h"

namespace {

void send_json(WebRequest &server, uint16_t status, ArduinoJson::JsonDocument &doc) {
    String json;
    serializeJson(doc, json);
    WebResponseUtils::sendNoStoreHeaders(server);
    server.send(status, "application/json", json);
}

void send_ota_busy_if_needed(WebRequest &server, bool ota_busy, const char *ota_busy_json) {
    if (!ota_busy) {
        return;
    }
    WebResponseUtils::sendNoStoreHeaders(server);
    server.send(503, "application/json", ota_busy_json);
}

}  // namespace

namespace WebThresholdApiHandlers {

void handleState(WebHandlerContext &context, bool ota_busy, const char *ota_busy_json) {
    if (!context.server || !context.display_thresholds) {
        return;
    }
    if (ota_busy) {
        send_ota_busy_if_needed(*context.server, ota_busy, ota_busy_json);
        return;
    }

    ArduinoJson::JsonDocument doc;
    WebThresholdApiUtils::fillStateJson(doc.to<ArduinoJson::JsonObject>(),
                                        context.display_thresholds->snapshot(),
                                        DisplayThresholds::defaults());
    send_json(*context.server, 200, doc);
}

void handleUpdate(WebHandlerContext &context, bool ota_busy, const char *ota_busy_json) {
    if (!context.server || !context.display_thresholds) {
        return;
    }
    if (ota_busy) {
        send_ota_busy_if_needed(*context.server, ota_busy, ota_busy_json);
        return;
    }

    const DisplayThresholds::Config current = context.display_thresholds->snapshot();
    const WebThresholdApiUtils::ParseResult parse_result =
        WebThresholdApiUtils::parseUpdateRequestBody(context.server->arg("plain"), current);
    if (!parse_result.success) {
        ArduinoJson::JsonDocument doc;
        WebThresholdApiUtils::fillErrorJson(doc.to<ArduinoJson::JsonObject>(),
                                            parse_result.error_message.c_str());
        send_json(*context.server, parse_result.status_code, doc);
        return;
    }

    String error;
    if (!context.display_thresholds->apply(parse_result.update, &error)) {
        ArduinoJson::JsonDocument doc;
        WebThresholdApiUtils::fillErrorJson(doc.to<ArduinoJson::JsonObject>(),
                                            error.length() ? error.c_str() : "Failed to save thresholds");
        send_json(*context.server, 500, doc);
        return;
    }

    ArduinoJson::JsonDocument doc;
    WebThresholdApiUtils::fillSuccessJson(doc.to<ArduinoJson::JsonObject>(),
                                          context.display_thresholds->snapshot());
    send_json(*context.server, 200, doc);
}

void handleReset(WebHandlerContext &context, bool ota_busy, const char *ota_busy_json) {
    if (!context.server || !context.display_thresholds) {
        return;
    }
    if (ota_busy) {
        send_ota_busy_if_needed(*context.server, ota_busy, ota_busy_json);
        return;
    }

    String error;
    if (!context.display_thresholds->reset(&error)) {
        ArduinoJson::JsonDocument doc;
        WebThresholdApiUtils::fillErrorJson(doc.to<ArduinoJson::JsonObject>(),
                                            error.length() ? error.c_str() : "Failed to reset thresholds");
        send_json(*context.server, 500, doc);
        return;
    }

    ArduinoJson::JsonDocument doc;
    WebThresholdApiUtils::fillSuccessJson(doc.to<ArduinoJson::JsonObject>(),
                                          context.display_thresholds->snapshot());
    send_json(*context.server, 200, doc);
}

}  // namespace WebThresholdApiHandlers
