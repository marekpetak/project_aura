#include <unity.h>

#include <stdio.h>
#include <string.h>

#include "core/MqttEventQueue.h"

namespace {

Logger::RecentEntry make_entry(uint32_t ms, Logger::Level level, const char *tag, const char *message) {
    Logger::RecentEntry entry{};
    entry.ms = ms;
    entry.level = level;
    if (tag) {
        strncpy(entry.tag, tag, sizeof(entry.tag) - 1);
        entry.tag[sizeof(entry.tag) - 1] = '\0';
    }
    if (message) {
        strncpy(entry.message, message, sizeof(entry.message) - 1);
        entry.message[sizeof(entry.message) - 1] = '\0';
    }
    return entry;
}

} // namespace

void setUp() {
    MqttEventQueue::instance().clear();
}

void tearDown() {
    MqttEventQueue::instance().clear();
}

void test_queue_keeps_fifo_order() {
    MqttEventQueue::instance().enqueue(make_entry(10, Logger::Info, "WiFi", "first"));
    MqttEventQueue::instance().enqueue(make_entry(20, Logger::Warn, "MQTT", "second"));

    Logger::RecentEntry entry{};
    TEST_ASSERT_TRUE(MqttEventQueue::instance().pop(entry));
    TEST_ASSERT_EQUAL_UINT32(10, entry.ms);
    TEST_ASSERT_EQUAL_STRING("first", entry.message);
    TEST_ASSERT_TRUE(MqttEventQueue::instance().pop(entry));
    TEST_ASSERT_EQUAL_UINT32(20, entry.ms);
    TEST_ASSERT_EQUAL_STRING("second", entry.message);
}

void test_queue_overwrites_oldest_when_full() {
    for (uint32_t i = 0; i < 30; ++i) {
        char message[16];
        snprintf(message, sizeof(message), "msg%lu", static_cast<unsigned long>(i));
        MqttEventQueue::instance().enqueue(make_entry(i, Logger::Info, "WiFi", message));
    }

    TEST_ASSERT_EQUAL_UINT32(24, MqttEventQueue::instance().size());

    Logger::RecentEntry entry{};
    TEST_ASSERT_TRUE(MqttEventQueue::instance().pop(entry));
    TEST_ASSERT_EQUAL_UINT32(6, entry.ms);
    TEST_ASSERT_EQUAL_STRING("msg6", entry.message);
}

void test_queue_capture_pause_suppresses_enqueue_if_capturing() {
    {
        MqttEventQueue::CapturePause capture_pause;
        TEST_ASSERT_FALSE(MqttEventQueue::instance().enqueueIfCapturing(
            make_entry(1, Logger::Warn, "MQTT", "suppressed")));
    }

    TEST_ASSERT_EQUAL_UINT32(0, MqttEventQueue::instance().size());
    TEST_ASSERT_TRUE(MqttEventQueue::instance().enqueueIfCapturing(
        make_entry(2, Logger::Warn, "MQTT", "accepted")));
    TEST_ASSERT_EQUAL_UINT32(1, MqttEventQueue::instance().size());
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_queue_keeps_fifo_order);
    RUN_TEST(test_queue_overwrites_oldest_when_full);
    RUN_TEST(test_queue_capture_pause_suppresses_enqueue_if_capturing);
    return UNITY_END();
}