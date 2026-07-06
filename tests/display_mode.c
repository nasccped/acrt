#include "../src/acrt.h"
#include "test_util.h"

// Asserts if two display modes refers to the same variant.
#define ASSERT_DISPLAY_MODE(EXPECTED, GOT)                                     \
  ASSERT_EQ_INT("DisplayMode", (int)(EXPECTED), (int)(GOT));

// Returns the inner display mode from an acrt struct.
#define GET_DISPLAY_MODE(ACRT) (ACRT).__context.display_mode

static acrt_t acrt;

assertion_result_t test_case_01();

int main() {
  GENERATE_TEST_CASES(tests, CAST_TO_ASSERT_FUNCTION(test_case_01));
  RUN_TEST_CASES(tests, NULL);

  return 0;
}

assertion_result_t test_case_01() {
  acrt = ACRT_NEW();
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_FAILED_ONLY, GET_DISPLAY_MODE(acrt));

  acrt_set_display_mode_to_all(&acrt);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_ALL, GET_DISPLAY_MODE(acrt));

  acrt_set_display_mode_to_quiet(&acrt);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_QUIET, GET_DISPLAY_MODE(acrt));

  acrt_set_display_mode_to_failed_only(&acrt);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_FAILED_ONLY, GET_DISPLAY_MODE(acrt));

  return ASSERTION_PASSED;
}
