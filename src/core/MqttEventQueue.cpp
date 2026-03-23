// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "core/MqttEventQueue.h"

MqttEventQueue::CapturePause::CapturePause() {
    MqttEventQueue::instance().pauseCapture();
}

MqttEventQueue::CapturePause::~CapturePause() {
    MqttEventQueue::instance().resumeCapture();
}

MqttEventQueue &MqttEventQueue::instance() {
    static MqttEventQueue queue;
    return queue;
}

MqttEventQueue::MqttEventQueue() {
#ifndef UNIT_TEST
    mutex_ = xSemaphoreCreateMutexStatic(&mutex_buffer_);
#endif
}

void MqttEventQueue::clear() {
    lock();
    head_ = 0;
    count_ = 0;
    unlock();
}

bool MqttEventQueue::enqueueIfCapturing(const Logger::RecentEntry &entry) {
    if (!captureEnabled()) {
        return false;
    }
    return enqueue(entry);
}

bool MqttEventQueue::enqueue(const Logger::RecentEntry &entry) {
    lock();
    entries_[head_] = entry;
    head_ = (head_ + 1) % kCapacity;
    if (count_ < kCapacity) {
        count_++;
    }
    unlock();
    return true;
}

bool MqttEventQueue::hasPending() const {
    return size() > 0;
}

size_t MqttEventQueue::size() const {
    lock();
    const size_t count = count_;
    unlock();
    return count;
}

bool MqttEventQueue::peek(Logger::RecentEntry &out) const {
    lock();
    if (count_ == 0) {
        unlock();
        return false;
    }
    const size_t start = (head_ + kCapacity - count_) % kCapacity;
    out = entries_[start];
    unlock();
    return true;
}

bool MqttEventQueue::discardFront() {
    lock();
    if (count_ == 0) {
        unlock();
        return false;
    }
    count_--;
    unlock();
    return true;
}

bool MqttEventQueue::pop(Logger::RecentEntry &out) {
    lock();
    if (count_ == 0) {
        unlock();
        return false;
    }
    const size_t start = (head_ + kCapacity - count_) % kCapacity;
    out = entries_[start];
    count_--;
    unlock();
    return true;
}

void MqttEventQueue::lock() const {
#ifdef UNIT_TEST
    mutex_.lock();
#else
    if (mutex_) {
        xSemaphoreTake(mutex_, portMAX_DELAY);
    }
#endif
}

void MqttEventQueue::unlock() const {
#ifdef UNIT_TEST
    mutex_.unlock();
#else
    if (mutex_) {
        xSemaphoreGive(mutex_);
    }
#endif
}

void MqttEventQueue::pauseCapture() {
    capture_pause_depth_.fetch_add(1, std::memory_order_acq_rel);
}

void MqttEventQueue::resumeCapture() {
    const uint32_t current = capture_pause_depth_.load(std::memory_order_acquire);
    if (current == 0) {
        return;
    }
    capture_pause_depth_.fetch_sub(1, std::memory_order_acq_rel);
}

bool MqttEventQueue::captureEnabled() const {
    return capture_pause_depth_.load(std::memory_order_acquire) == 0;
}
