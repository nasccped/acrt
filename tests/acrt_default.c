// Include c file since we're looking for private func impls
#include "../src/acrt.h"
#include "test_util.h"

#define CTX_NAME(ACRT) (ACRT).context_name

assertion_result_t context_name();
assertion_result_t counting();
assertion_result_t display_mode();
assertion_result_t on_fail();
assertion_result_t previous_assertion_failed();

static acrt_t acrt;

int main() {
  acrt = ACRT_DEFAULT;
  GENERATE_TEST_CASES(tests, CAST_TO_ASSERT_FUNCTION(context_name),
                      CAST_TO_ASSERT_FUNCTION(counting),
                      CAST_TO_ASSERT_FUNCTION(display_mode),
                      CAST_TO_ASSERT_FUNCTION(on_fail),
                      CAST_TO_ASSERT_FUNCTION(previous_assertion_failed));
  RUN_TEST_CASES(tests, NULL);

  return 0;
}

assertion_result_t context_name() {
  ASSERT_EQ_INT("ContextNameKind", (int)CONTEXT_NAME_USE_FILE_NAME,
                (int)CTX_NAME(acrt).kind);
  ASSERT_EQ_STRING("ContextNameFile", __FILE__, CTX_NAME(acrt).data.file);
  ASSERT("ContextNameCustom", CTX_NAME(acrt).data.custom == NULL);

  return ASSERTION_PASSED;
}

assertion_result_t counting() {
  ASSERT_EQ_INT("CountingPassedWithoutWarnings", 0,
                acrt.counting.passed.without_warnings);
  ASSERT_EQ_INT("CountingPassedWithWarnings", 0,
                acrt.counting.passed.with_warnings);
  ASSERT_EQ_INT("CountingFailed", 0, acrt.counting.failed);
  ASSERT_EQ_INT("CountingIgnored", 0, acrt.counting.ignored);
  ASSERT_EQ_INT("CountingTotal", 0, acrt.counting.total);

  return ASSERTION_PASSED;
}

assertion_result_t display_mode() {
  ASSERT_EQ_INT("DisplayMode", (int)DISPLAY_MODE_FAILED_ONLY,
                acrt.display_mode);

  return ASSERTION_PASSED;
}

assertion_result_t on_fail() {
  ASSERT_EQ_INT("OnFailActionKind", (int)ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,
                acrt.on_fail.action_kind);
  ASSERT_EQ_INT("OnFailExitCode", 1, acrt.on_fail.data.code);

  return ASSERTION_PASSED;
}
assertion_result_t previous_assertion_failed() {
  ASSERT_EQ_INT("PreviousAssertionFailed", 0, acrt.previous_assertion_failed);

  return ASSERTION_PASSED;
}
