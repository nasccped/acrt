#include "../src/acrt_result.h"
#include "test_util.h"

typedef struct __acrt_result result_t;

// Asserts if the 'result.context.line' is equals to the expected.
#define ASSERT_CONTEXT_LINE(EXPECTED, GOT)                                     \
  ASSERT_EQ_INT("ResultContextLine", (EXPECTED), (GOT))

// Asserts if the 'result.context.name' is equals to the expected.
#define ASSERT_CONTEXT_NAME(EXPECTED, GOT)                                     \
  ASSERT_EQ_STRING("ResultContextNmae", (EXPECTED), (GOT))

// Just asserts data fields. Since data is a union, the result must be got in
// place (on the macro call).
#define ASSERT_DATA(RESULT) ASSERT("ResultData", (RESULT))

// Asserts if the context kind enum coercion is equals to the expected integer.
#define ASSERT_KIND(EXPECTED, GOT)                                             \
  ASSERT_EQ_INT("ResultKind", (int)(EXPECTED), (int)(GOT))

// Asserts if the result is passed (success).
#define ASSERT_PASSED(EXPECTED, GOT)                                           \
  ASSERT_EQ_INT("ResultPassed", (EXPECTED), (GOT))

// Context line related assertions.
assertion_result_t context_line();

// Context name related assertions.
assertion_result_t context_name();

// Inner data related assertions.
assertion_result_t data();

// If the assertion is passed.
assertion_result_t passed();

// Result kind related assertions.
assertion_result_t result_kind();

static result_t result;

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
  ASSERT_CONTEXT_LINE(2, result.context.line);

  result = acrt_result_from_int("", 0, 0);
  ASSERT_CONTEXT_LINE(0, result.context.line);

  result = acrt_result_from_int("", 20, 0);
  ASSERT_CONTEXT_LINE(20, result.context.line);

  return ASSERTION_PASSED;
}

assertion_result_t context_name() {
  result = acrt_result_from_int(__FUNCTION__, 1, 42);
  // NOTE: since expected value is hardcoded, it needs to changed whenever func
  //       name also changes.
  ASSERT_CONTEXT_NAME("context_name", result.context.name);

  result = acrt_result_from_int("a", 0, 0);
  ASSERT_CONTEXT_NAME("a", result.context.name);

  return ASSERTION_PASSED;
}

assertion_result_t data(void) {
  char *string = "Some string";
  result = acrt_result_from_single_pointer("", 2, string);
  ASSERT_DATA(result.data.single_pointer == string);

  return ASSERTION_PASSED;
}

assertion_result_t passed(void) {
  result = acrt_result_from_int("", 2, 2);
  ASSERT_PASSED(1, result.passed);

  result = acrt_result_from_int("", 2, 0);
  ASSERT_PASSED(0, result.passed);

  result = acrt_result_from_single_pointer("", 2, "temp string");
  ASSERT_PASSED(1, result.passed);

  result = acrt_result_from_single_pointer("", 2, NULL);
  ASSERT_PASSED(0, result.passed);

  return ASSERTION_PASSED;
}

assertion_result_t result_kind(void) {
  result = acrt_result_from_int("", 1, 1);
  ASSERT_KIND(SIMPLE_BOOLEAN_ASSERTION_KIND, result.kind);

  result = acrt_result_from_single_pointer("", 1, NULL);
  ASSERT_KIND(POINTER_BOOLEAN_ASSERTION_KIND, result.kind);

  return ASSERTION_PASSED;
}
