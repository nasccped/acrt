#include <stdio.h>
#include <string.h>

#include "../src/acrt_result.h"
#include "test_util.h"

typedef struct __acrt_result result_t;
typedef int (*func_t)(void);

static result_t result;

// Asserts if the 'result.context.line' is equals to the expected.
int assert_context_line(unsigned int expected);

// Asserts if the 'result.context.name' is equals to the expected.
int assert_context_name(const char *expected);

// Asserts if the context kind enum coercion is equals to the expected integer.
int assert_kind(int expected);

// Asserts if the result is passed (success).
int assert_passed(int expected);

int test_case_01();

int test_case_02();

int main() {

  func_t test_cases[] = {test_case_01, test_case_02};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    func_t func = test_cases[i];
    if (func())
      return 1;
  }

  return 0;
}

int assert_context_name(const char *expected) {
  ASSERT_EQ_STRING("ResultContextName", expected, result.context.name);
  return ASSERTION_PASSED;
}

int assert_context_line(unsigned int expected) {
  ASSERT_EQ_INT("ResultContextLine", expected, result.context.line);
  return ASSERTION_PASSED;
}

int assert_kind(int expected) {
  ASSERT_EQ_INT("ResultContextKind", expected, (int)result.kind);
  return ASSERTION_PASSED;
}

int assert_passed(int passed) {
  ASSERT_EQ_INT("ResultIsPassed", !!passed, !!result.passed);
  return ASSERTION_PASSED;
}

int test_case_01() {
  result = acrt_result_from_int(__FUNCTION__, 1, 42);

  if (!assert_context_name("test_case_01"))
    return 1;
  else if (!assert_context_line(1))
    return 1;
  else if (!assert_kind(SIMPLE_BOOLEAN_ASSERTION_KIND))
    return 1;
  else if (!assert_passed(1))
    return 1;

  result = acrt_result_from_int("a", 11, 0);

  if (!assert_context_name("a"))
    return 1;
  else if (!assert_context_line(11))
    return 1;
  else if (!assert_kind(SIMPLE_BOOLEAN_ASSERTION_KIND))
    return 1;
  else if (!assert_passed(0))
    return 1;

  return 0;
}

int test_case_02() {
  result = acrt_result_from_single_pointer("custom name", 2, NULL);

  if (!assert_context_name("custom name"))
    return 1;
  else if (!assert_context_line(2))
    return 1;
  else if (!assert_kind(POINTER_BOOLEAN_ASSERTION_KIND))
    return 1;
  else if (!assert_passed(0))
    return 1;

  result = acrt_result_from_single_pointer("other name", 3, "char pointer");

  if (!assert_context_name("other name"))
    return 1;
  else if (!assert_context_line(3))
    return 1;
  else if (!assert_kind(POINTER_BOOLEAN_ASSERTION_KIND))
    return 1;
  else if (!assert_passed(1))
    return 1;

  return 0;
}
