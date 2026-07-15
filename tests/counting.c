#include "../src/acrt.h"
#include "test_util.h"

// Set default config for this local testing.
#define ACRT_SET_LOCAL_DEFAULT(CONTINUE_ON_FAIL)                               \
  do {                                                                         \
    acrt_reset_assertions_state(&acrt);                                        \
    acrt_set_display_mode_to_quiet(&acrt);                                     \
    if ((CONTINUE_ON_FAIL))                                                    \
      acrt_set_on_fail_continue_assertions(&acrt);                             \
    else                                                                       \
      acrt_set_on_fail_disable_assertions(&acrt);                              \
  } while (0)

// Asserts the total field of the counting struct.
#define ASSERT_TOTAL(EXPECTED)                                                 \
  ASSERT_EQ_INT("CountingTotal", (EXPECTED), acrt.counting.total)

// Asserts the passed field of the counting struct. It takes a boolean warning
// trigger (if it was passed with(out) warnings).
#define ASSERT_PASSED(EXPECTED, WARNING)                                       \
  ASSERT_EQ_INT("CountingPassed", (EXPECTED),                                  \
                (WARNING) ? acrt.counting.passed.with_warnings                 \
                          : acrt.counting.passed.without_warnings)

// Asserts the failed field of the counting struct.
#define ASSERT_FAILED(EXPECTED)                                                \
  ASSERT_EQ_INT("CountingFailed", (EXPECTED), acrt.counting.failed)

// Asserts the ignored field of the counting struct.
#define ASSERT_IGNORED(EXPECTED)                                               \
  ASSERT_EQ_INT("CountingIgnored", (EXPECTED), acrt.counting.ignored)

#define RUN_NUMBER_ASSERTION(INT) ACRT_BOOL(&acrt, (int)(INT))
#define RUN_POINTER_ASSERTION(PTR) ACRT_BOOL(&acrt, (PTR))

#define WITHOUT_WARNINGS 0
#define WITH_WARNINGS 1

static acrt_t acrt;

assertion_result_t failed();
assertion_result_t ignored();
assertion_result_t passed();
assertion_result_t total();

int main() {
  acrt = ACRT_DEFAULT;
  RUN_TESTS(failed, ignored, passed, total);
  return 0;
}

assertion_result_t failed() {
  ACRT_SET_LOCAL_DEFAULT(1);

  // Count how many failed assertions
  RUN_NUMBER_ASSERTION(0);
  RUN_POINTER_ASSERTION(NULL);
  ASSERT_FAILED(2);

  // Count failed assertions still the same even after success
  RUN_NUMBER_ASSERTION(1.0);
  RUN_POINTER_ASSERTION("string");
  ASSERT_FAILED(2);

  return ASSERTION_PASSED;
}

assertion_result_t ignored() {
  ACRT_SET_LOCAL_DEFAULT(0);

  // One passes, one fail
  RUN_NUMBER_ASSERTION(1.2345);
  RUN_NUMBER_ASSERTION(0);
  ASSERT_PASSED(1, WITHOUT_WARNINGS);
  ASSERT_FAILED(1);
  ASSERT_IGNORED(0);

  // All ignored since latest one was failed
  RUN_NUMBER_ASSERTION(1);
  RUN_NUMBER_ASSERTION(0);
  RUN_POINTER_ASSERTION("a");
  RUN_POINTER_ASSERTION(NULL);
  ASSERT_PASSED(1, WITHOUT_WARNINGS);
  ASSERT_FAILED(1);
  ASSERT_IGNORED(4);

  return ASSERTION_PASSED;
}

assertion_result_t passed() {
  ACRT_SET_LOCAL_DEFAULT(1);

  RUN_NUMBER_ASSERTION(1);
  RUN_POINTER_ASSERTION(1);
  ASSERT_PASSED(2, WITHOUT_WARNINGS);

  return ASSERTION_PASSED;
}

assertion_result_t total() {
  ACRT_SET_LOCAL_DEFAULT(0);

  RUN_NUMBER_ASSERTION(1);
  RUN_NUMBER_ASSERTION(0.0);
  RUN_POINTER_ASSERTION(NULL);
  RUN_POINTER_ASSERTION("other string");
  ASSERT_TOTAL(4);

  return ASSERTION_PASSED;
}
