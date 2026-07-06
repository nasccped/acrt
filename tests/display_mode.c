#include "../src/acrt.h"
#include "test_util.h"

// Asserts if two display modes refers to the same variant.
#define ASSERT_DISPLAY_MODE(EXPECTED, GOT)                                     \
  ASSERT_EQ_INT("DisplayMode", (int)(EXPECTED), (int)(GOT));

static acrt_t acrt;

assertion_result_t test_case_01();

int main() {
  GENERATE_TEST_CASES(tests, CAST_TO_ASSERT_FUNCTION(test_case_01));
  RUN_TEST_CASES(tests, NULL);

  return 0;
}

assertion_result_t test_case_01() {
  acrt = ACRT_NEW();
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_FAILED_ONLY, acrt.__context.display_mode);

  acrt_display_mode(&acrt, DISPLAY_MODE_ALL);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_ALL, acrt.__context.display_mode);

  acrt_display_mode(&acrt, DISPLAY_MODE_QUIET);
  ASSERT_DISPLAY_MODE(DISPLAY_MODE_QUIET, acrt.__context.display_mode);

  return ASSERTION_PASSED;
}
