// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "modules/DisplayThresholds.h"

namespace WebThresholdApiUtils {

struct ParseResult {
    bool success = false;
    uint16_t status_code = 400;
    String error_message;
    DisplayThresholds::Config update{};
};

void fillStateJson(ArduinoJson::JsonObject root,
                   const DisplayThresholds::Config &active,
                   const DisplayThresholds::Config &factory);
void fillSuccessJson(ArduinoJson::JsonObject root,
                     const DisplayThresholds::Config &active);
void fillErrorJson(ArduinoJson::JsonObject root, const char *message);
ParseResult parseUpdateRequestBody(const String &body,
                                   const DisplayThresholds::Config &current);

}  // namespace WebThresholdApiUtils
