#include "../src/acrt_result.h"
#include "test_util.h"

#define ASSERT_CONTEXT_LINE(EXPECTED)                                          \
  ASSERT_EQ_INT("ResultContextLine", (EXPECTED), result.context.line)

#define ASSERT_CONTEXT_NAME(EXPECTED)                                          \
  ASSERT_EQ_STRING("ResultContextNmae", (EXPECTED), result.context.name)

// Just asserts data fields. Since data is a union, the result must be got in
// place (on the macro call).
#define ASSERT_DATA(RESULT) ASSERT("ResultData", (RESULT))

#define ASSERT_KIND(EXPECTED)                                                  \
  ASSERT_EQ_INT("ResultKind", (int)(EXPECTED), (int)result.kind)

#define ASSERT_PASSED(EXPECTED)                                                \
  ASSERT_EQ_INT("ResultPassed", (EXPECTED), acrt_result_is_passed(&result))

assertion_result_t context_line();
assertion_result_t context_name();
assertion_result_t data();
assertion_result_t passed();
assertion_result_t result_kind();

static acrt_result_t result;

int main() {
  GENERATE_TEST_CASES(
      tests, CAST_TO_ASSERT_FUNCTION(context_line),
      CAST_TO_ASSERT_FUNCTION(context_name),
      CAST_TO_ASSERT_FUNCTION(result_kind), CAST_TO_ASSERT_FUNCTION(data),
      CAST_TO_ASSERT_FUNCTION(passed), CAST_TO_ASSERT_FUNCTION(result_kind));

  RUN_TEST_CASES(tests, NULL);
  return 0;
}

assertion_result_t context_line() {
  result = acrt_result_from_int("", 2, 0);
  ASSERT_CONTEXT_LINE(2);

  result = acrt_result_from_int("", 0, 0);
  ASSERT_CONTEXT_LINE(0);

  result = acrt_result_from_int("", 20, 0);
  ASSERT_CONTEXT_LINE(20);

  return ASSERTION_PASSED;
}

assertion_result_t context_name() {
  result = acrt_result_from_int(__FUNCTION__, 1, 42);
  // NOTE: since expected value is hardcoded, it needs to changed whenever func
  //       name also changes.
  ASSERT_CONTEXT_NAME("context_name");

  result = acrt_result_from_int("a", 0, 0);
  ASSERT_CONTEXT_NAME("a");

  return ASSERTION_PASSED;
}

assertion_result_t data(void) {
  char *string = "Some string";
  result = acrt_result_from_single_pointer("", 2, string);
  ASSERT_DATA(result.data.single_pointer == string);

  result = acrt_result_from_int("", 22, 2);
  ASSERT_DATA(result.data.integer_cast == 2);

  result = acrt_result_from_int("", 22, (int)7.99);
  ASSERT_DATA(result.data.integer_cast == 7);

  return ASSERTION_PASSED;
}

assertion_result_t passed(void) {
  result = acrt_result_from_int("", 2, 2);
  ASSERT_PASSED(1);

  result = acrt_result_from_int("", 2, 0);
  ASSERT_PASSED(0);

  result = acrt_result_from_single_pointer("", 2, "temp string");
  ASSERT_PASSED(1);

  result = acrt_result_from_single_pointer("", 2, NULL);
  ASSERT_PASSED(0);

  return ASSERTION_PASSED;
}

assertion_result_t result_kind(void) {
  result = acrt_result_from_int("", 1, 1);
  ASSERT_KIND(INTEGER_BOOLEAN_ASSERTION_KIND);

  result = acrt_result_from_single_pointer("", 1, NULL);
  ASSERT_KIND(POINTER_BOOLEAN_ASSERTION_KIND);

  return ASSERTION_PASSED;
}
