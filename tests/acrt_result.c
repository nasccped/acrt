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
  RUN_TESTS(context_line, context_name, result_kind, data, passed);
  return 0;
}

assertion_result_t context_line() {
  result = acrt_result_new_bool("", 2, 0, 0);
  ASSERT_CONTEXT_LINE(2);

  result = acrt_result_new_bool("", 0, 0, 0);
  ASSERT_CONTEXT_LINE(0);

  result = acrt_result_new_bool("", 20, 0, 0);
  ASSERT_CONTEXT_LINE(20);

  return ASSERTION_PASSED;
}

assertion_result_t context_name() {
  result = acrt_result_new_bool(__FUNCTION__, 1, 42, 0);
  // NOTE: since expected value is hardcoded, it needs to changed whenever func
  //       name also changes.
  ASSERT_CONTEXT_NAME("context_name");

  result = acrt_result_new_bool("a", 0, 0, 0);
  ASSERT_CONTEXT_NAME("a");

  return ASSERTION_PASSED;
}

assertion_result_t data(void) {
  char *string = "Some string";
  result = acrt_result_new_bool("", 2, (uintptr_t)string, 1);
  ASSERT_DATA(result.data.single_pointer == string);

  result = acrt_result_new_bool("", 22, 2, 0);
  ASSERT_DATA(result.data.integer_cast == 2);

  result = acrt_result_new_bool("", 22, (uintptr_t)7.99, 0);
  ASSERT_DATA(result.data.integer_cast == 7);

  return ASSERTION_PASSED;
}

assertion_result_t passed(void) {
  result = acrt_result_new_bool("", 2, 2, 0);
  ASSERT_PASSED(1);

  result = acrt_result_new_bool("", 2, 0, 0);
  ASSERT_PASSED(0);

  result = acrt_result_new_bool("", 2, (uintptr_t)"temp string", 1);
  ASSERT_PASSED(1);

  result = acrt_result_new_bool("", 2, (uintptr_t)NULL, 1);
  ASSERT_PASSED(0);

  return ASSERTION_PASSED;
}

assertion_result_t result_kind(void) {
  result = acrt_result_new_bool("", 1, 1, 0);
  ASSERT_KIND(INTEGER_BOOLEAN_ASSERTION_KIND);

  result = acrt_result_new_bool("", 1, (uintptr_t)NULL, 1);
  ASSERT_KIND(POINTER_BOOLEAN_ASSERTION_KIND);

  return ASSERTION_PASSED;
}
