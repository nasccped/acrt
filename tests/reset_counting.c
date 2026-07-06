#include "../src/acrt.h"
#include "test_util.h"

// Set default config for this local testing.
#define ACRT_SET_LOCAL_DEFAULT(ACRT, CONTINUE_ON_FAIL)                         \
  do {                                                                         \
    acrt_reset_counting((ACRT));                                               \
    acrt_set_display_mode_to_quiet((ACRT));                                    \
    if ((CONTINUE_ON_FAIL))                                                    \
      acrt_on_fail_continue_assertions((ACRT));                                \
    else                                                                       \
      acrt_on_fail_skip_assertions((ACRT));                                    \
  } while (0);

// Asserts the total field of the counting struct.
#define ASSERT_TOTAL(EXPECTED, GOT)                                            \
  ASSERT_EQ_INT("CountingTotal", (EXPECTED), (GOT))

// Asserts the passed field of the counting struct.
#define ASSERT_PASSED(EXPECTED, GOT)                                           \
  ASSERT_EQ_INT("CountingPassed", (EXPECTED), (GOT))

// Asserts the failed field of the counting struct.
#define ASSERT_FAILED(EXPECTED, GOT)                                           \
  ASSERT_EQ_INT("CountingFailed", (EXPECTED), (GOT))

// Asserts the ignored field of the counting struct.
#define ASSERT_IGNORED(EXPECTED, GOT)                                          \
  ASSERT_EQ_INT("CountingIgnored", (EXPECTED), (GOT))

static acrt_t acrt;

assertion_result_t failed();
assertion_result_t ignored();
assertion_result_t passed();
assertion_result_t total();

int main() {
  GENERATE_TEST_CASES(
      tests, CAST_TO_ASSERT_FUNCTION(failed), CAST_TO_ASSERT_FUNCTION(ignored),
      CAST_TO_ASSERT_FUNCTION(passed), CAST_TO_ASSERT_FUNCTION(total));
  RUN_TEST_CASES(tests, NULL);

  return 0;
}

// NOTE: test cases still not impl since they requir RUN macros impl.

assertion_result_t failed() {
  acrt = ACRT_NEW();
  ACRT_SET_LOCAL_DEFAULT(&acrt, 1);

  // TODO: add assertions here.
  return ASSERTION_PASSED;
}

assertion_result_t ignored() {
  acrt = ACRT_NEW();
  ACRT_SET_LOCAL_DEFAULT(&acrt, 1);

  // TODO: add assertions here.
  return ASSERTION_PASSED;
}

assertion_result_t passed() {
  acrt = ACRT_NEW();
  ACRT_SET_LOCAL_DEFAULT(&acrt, 1);

  // TODO: add assertions here.
  return ASSERTION_PASSED;
}

assertion_result_t total() {
  acrt = ACRT_NEW();
  ACRT_SET_LOCAL_DEFAULT(&acrt, 1);

  // TODO: add assertions here.
  return ASSERTION_PASSED;
}
