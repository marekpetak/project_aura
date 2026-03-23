// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#pragma once

#include <atomic>
#include <stddef.h>

#ifdef UNIT_TEST
#include <mutex>
#else
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#endif

#include "core/Logger.h"

class MqttEventQueue {
public:
    class CapturePause {
    public:
        CapturePause();
        ~CapturePause();

        CapturePause(const CapturePause &) = delete;
        CapturePause &operator=(const CapturePause &) = delete;
    };

    static MqttEventQueue &instance();

    void clear();
    bool enqueueIfCapturing(const Logger::RecentEntry &entry);
    bool enqueue(const Logger::RecentEntry &entry);
    bool hasPending() const;
    size_t size() const;
    bool peek(Logger::RecentEntry &out) const;
    bool discardFront();
    bool pop(Logger::RecentEntry &out);

private:
    MqttEventQueue();

    void lock() const;
    void unlock() const;
    void pauseCapture();
    void resumeCapture();
    bool captureEnabled() const;

    static constexpr size_t kCapacity = 24;

#ifdef UNIT_TEST
    mutable std::mutex mutex_{};
#else
    mutable StaticSemaphore_t mutex_buffer_{};
    mutable SemaphoreHandle_t mutex_ = nullptr;
#endif
    Logger::RecentEntry entries_[kCapacity]{};
    size_t head_ = 0;
    size_t count_ = 0;
    std::atomic<uint32_t> capture_pause_depth_{0};
};
