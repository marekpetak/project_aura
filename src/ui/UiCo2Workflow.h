// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#pragma once

#include <stdint.h>

namespace UiCo2Workflow {

enum class OverlayMode : uint8_t {
    Hidden = 0,
    Confirm,
    CalibrationProgress,
    AscProgress,
};

enum class OverlayTransition : uint8_t {
    ShowConfirm = 0,
    ShowCalibrationProgress,
    ShowAscProgress,
    Hide,
};

struct AscApplyPlan {
    bool runtime_enabled;
    bool config_enabled;
    bool persist_now;
    bool attempt_live_apply;
};

constexpr OverlayMode nextOverlayMode(OverlayMode current, OverlayTransition transition) {
    (void)current;
    switch (transition) {
        case OverlayTransition::ShowConfirm:
            return OverlayMode::Confirm;
        case OverlayTransition::ShowCalibrationProgress:
            return OverlayMode::CalibrationProgress;
        case OverlayTransition::ShowAscProgress:
            return OverlayMode::AscProgress;
        case OverlayTransition::Hide:
        default:
            return OverlayMode::Hidden;
    }
}

constexpr AscApplyPlan beginAscApply(bool current_enabled,
                                     bool requested_enabled,
                                     bool sensor_ready) {
    if (requested_enabled == current_enabled) {
        return {current_enabled, current_enabled, false, false};
    }
    if (!sensor_ready) {
        return {requested_enabled, requested_enabled, true, false};
    }
    return {current_enabled, current_enabled, false, true};
}

constexpr AscApplyPlan finishAscApply(bool previous_enabled,
                                      bool requested_enabled,
                                      bool live_apply_ok) {
    if (live_apply_ok) {
        return {requested_enabled, requested_enabled, true, false};
    }
    return {previous_enabled, previous_enabled, false, false};
}

} // namespace UiCo2Workflow
