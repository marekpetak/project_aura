// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "web/WebApiUtils.h"

#include "core/SystemEventPolicy.h"

namespace WebApiUtils {

const char *eventLevelText(Logger::Level level) {
    return SystemEventPolicy::levelText(level);
}

const char *eventSeverityText(Logger::Level level) {
    return SystemEventPolicy::severityText(level);
}

bool shouldEmitWebEvent(const Logger::RecentEntry &entry) {
    return SystemEventPolicy::shouldEmit(entry);
}

String formatUptimeHuman(uint32_t uptime_seconds) {
    uint32_t days = uptime_seconds / 86400UL;
    uptime_seconds %= 86400UL;
    uint32_t hours = uptime_seconds / 3600UL;
    uptime_seconds %= 3600UL;
    uint32_t minutes = uptime_seconds / 60UL;

    char buf[32];
    if (days > 0) {
        snprintf(buf, sizeof(buf), "%lud %luh %lum",
                 static_cast<unsigned long>(days),
                 static_cast<unsigned long>(hours),
                 static_cast<unsigned long>(minutes));
    } else {
        snprintf(buf, sizeof(buf), "%luh %lum",
                 static_cast<unsigned long>(hours),
                 static_cast<unsigned long>(minutes));
    }
    return String(buf);
}

} // namespace WebApiUtils
