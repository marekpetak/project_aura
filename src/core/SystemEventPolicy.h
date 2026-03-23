// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#pragma once

#include "core/Logger.h"

namespace SystemEventPolicy {

const char *levelText(Logger::Level level);
const char *severityText(Logger::Level level);
bool shouldEmit(const Logger::RecentEntry &entry);
const char *typeText(const Logger::RecentEntry &entry);
const char *messageText(const Logger::RecentEntry &entry);

} // namespace SystemEventPolicy
