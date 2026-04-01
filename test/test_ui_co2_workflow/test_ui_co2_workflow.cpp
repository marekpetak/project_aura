#include <unity.h>

#include "ui/UiCo2Workflow.h"

using UiCo2Workflow::AscApplyPlan;
using UiCo2Workflow::OverlayMode;
using UiCo2Workflow::OverlayTransition;

void setUp() {}
void tearDown() {}

void test_begin_asc_apply_ready_keeps_current_state_until_live_apply_finishes() {
    const AscApplyPlan plan = UiCo2Workflow::beginAscApply(true, false, true);

    TEST_ASSERT_TRUE(plan.runtime_enabled);
    TEST_ASSERT_TRUE(plan.config_enabled);
    TEST_ASSERT_FALSE(plan.persist_now);
    TEST_ASSERT_TRUE(plan.attempt_live_apply);
}

void test_begin_asc_apply_not_ready_persists_requested_state_for_later() {
    const AscApplyPlan plan = UiCo2Workflow::beginAscApply(true, false, false);

    TEST_ASSERT_FALSE(plan.runtime_enabled);
    TEST_ASSERT_FALSE(plan.config_enabled);
    TEST_ASSERT_TRUE(plan.persist_now);
    TEST_ASSERT_FALSE(plan.attempt_live_apply);
}

void test_finish_asc_apply_success_commits_requested_state() {
    const AscApplyPlan plan = UiCo2Workflow::finishAscApply(true, false, true);

    TEST_ASSERT_FALSE(plan.runtime_enabled);
    TEST_ASSERT_FALSE(plan.config_enabled);
    TEST_ASSERT_TRUE(plan.persist_now);
    TEST_ASSERT_FALSE(plan.attempt_live_apply);
}

void test_finish_asc_apply_failure_restores_previous_state() {
    const AscApplyPlan plan = UiCo2Workflow::finishAscApply(true, false, false);

    TEST_ASSERT_TRUE(plan.runtime_enabled);
    TEST_ASSERT_TRUE(plan.config_enabled);
    TEST_ASSERT_FALSE(plan.persist_now);
    TEST_ASSERT_FALSE(plan.attempt_live_apply);
}

void test_overlay_transitions_follow_confirm_and_progress_flow() {
    OverlayMode mode = OverlayMode::Hidden;

    mode = UiCo2Workflow::nextOverlayMode(mode, OverlayTransition::ShowConfirm);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(OverlayMode::Confirm), static_cast<int>(mode));

    mode = UiCo2Workflow::nextOverlayMode(mode, OverlayTransition::ShowCalibrationProgress);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(OverlayMode::CalibrationProgress),
                          static_cast<int>(mode));

    mode = UiCo2Workflow::nextOverlayMode(mode, OverlayTransition::Hide);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(OverlayMode::Hidden), static_cast<int>(mode));

    mode = UiCo2Workflow::nextOverlayMode(mode, OverlayTransition::ShowAscProgress);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(OverlayMode::AscProgress), static_cast<int>(mode));

    mode = UiCo2Workflow::nextOverlayMode(mode, OverlayTransition::Hide);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(OverlayMode::Hidden), static_cast<int>(mode));
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_begin_asc_apply_ready_keeps_current_state_until_live_apply_finishes);
    RUN_TEST(test_begin_asc_apply_not_ready_persists_requested_state_for_later);
    RUN_TEST(test_finish_asc_apply_success_commits_requested_state);
    RUN_TEST(test_finish_asc_apply_failure_restores_previous_state);
    RUN_TEST(test_overlay_transitions_follow_confirm_and_progress_flow);
    return UNITY_END();
}
