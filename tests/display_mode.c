#include "../src/acrt.h"
#include "test_util.h"

// Asserts if display mode refers to the same as acrt inner variant.
#define ASSERT_DISPLAY_MODE(EXPECTED)                                          \
  ASSERT_EQ_INT("DisplayMode", (int)(EXPECTED), (int)acrt.display_mode);

static acrt_t acrt;

assertion_result_t test_case_01();

int main() {
  RUN_TESTS(test_case_01);
  return 0;
}

assertion_result_t test_case_01() {
  acrt = ACRT_DEFAULT;
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_FAILED_ONLY);

  acrt_set_display_mode_to_all(&acrt);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_ALL);

  acrt_set_display_mode_to_quiet(&acrt);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_QUIET);

  acrt_set_display_mode_to_failed_only(&acrt);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_FAILED_ONLY);

  return ASSERTION_PASSED;
}
