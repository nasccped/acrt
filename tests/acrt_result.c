#include <stdint.h>
#include <string.h>

#include "../src/acrt_result.h"
#include "test_util.h"

#define ASSERT_CONTEXT_LINE(EXPECTED)                                          \
  ASSERT_EQ_INT("ResultContextLine", (EXPECTED), result.context.line)

#define ASSERT_CONTEXT_NAME(EXPECTED)                                          \
  ASSERT_EQ_STRING("ResultContextName", (EXPECTED), result.context.name)

// Just asserts data fields. Since data is a union, the result must be got in
// place (on the macro call).
#define ASSERT_DATA(RESULT) ASSERT("ResultData", (RESULT))

#define ASSERT_KIND(EXPECTED)                                                  \
  ASSERT_EQ_INT("ResultKind", (int)(EXPECTED), (int)result.kind)

#define ASSERT_PASSED(EXPECTED)                                                \
  ASSERT_EQ_INT("ResultPassed", (EXPECTED), acrt_result_is_passed(&result))

#define ASSERT_STATUS(EXPECTED)                                                \
  ASSERT_EQ_INT("ResultStatus", (int)(EXPECTED), (int)result.status)

#define ASSERT_WARNING(VALUE) ASSERT("ResultWarning", (VALUE))

assertion_result_t context();
assertion_result_t data();
assertion_result_t passed();
assertion_result_t kind();
assertion_result_t warning();

// A function that always returns 0 (used on warning test case).
int always_fail(void *l, void *r);

// Returns 1 of both strings are non-null and equals (lexic).
int streq(char *s1, char *s2);

static acrt_result_t result;
static char *s1 = "string 1", *s2 = "string 2";

int main() {
  RUN_TESTS(context, kind, data, passed, warning);
  return 0;
}

assertion_result_t context() {
  result = acrt_result_new_bool("name", 11, 0, 0);
  ASSERT_CONTEXT_NAME("name");
  ASSERT_CONTEXT_LINE(11);

  result = acrt_result_new_eq("othername", 22, NULL, NULL, NULL);
  ASSERT_CONTEXT_NAME("othername");
  ASSERT_CONTEXT_LINE(22);

  return ASSERTION_PASSED;
}

// NOTE: this function only test value holder fields. Warnings and passed must
//       be checked at it's own test functions.
assertion_result_t data(void) {
  int integer;
  void *pointer, *left, *right;

  result = acrt_result_new_bool("", 22, 2, 0);
  integer = result.data.boolean_assertion.value.integer;
  ASSERT_DATA(integer == 2);

  result = acrt_result_new_bool("", 22, -4, 0);
  integer = result.data.boolean_assertion.value.integer;
  ASSERT_DATA(integer == -4);

  result = acrt_result_new_bool("", 2, (intptr_t)s1, 1);
  pointer = result.data.boolean_assertion.value.pointer;
  ASSERT_DATA(pointer == s1);

  result = acrt_result_new_bool("", 22, (intptr_t)7.99, 0);
  integer = result.data.boolean_assertion.value.integer;
  ASSERT_DATA(integer == 7);

  result = acrt_result_new_bool("", 22, (intptr_t)-7.99, 0);
  integer = result.data.boolean_assertion.value.integer;
  ASSERT_DATA(integer == -7);

  result = acrt_result_new_eq("", 0, s1, s2, (int (*)(void *, void *))streq);
  left = result.data.eq_assertion.params.left;
  right = result.data.eq_assertion.params.right;
  ASSERT_DATA(left == s1);
  ASSERT_DATA(right == s2);

  return ASSERTION_PASSED;
}

assertion_result_t passed(void) {
  result = acrt_result_new_bool("", 2, 2, 0);
  ASSERT_PASSED(1);

  result = acrt_result_new_bool("", 2, 0, 0);
  ASSERT_PASSED(0);

  result = acrt_result_new_bool("", 2, (intptr_t)s1, 1);
  ASSERT_PASSED(1);

  result = acrt_result_new_bool("", 2, (intptr_t)NULL, 1);
  ASSERT_PASSED(0);

  result =
      acrt_result_new_eq("", 2, "same", "same", (int (*)(void *, void *))streq);
  ASSERT_PASSED(1);

  result = acrt_result_new_eq("", 2, s1, s2, (int (*)(void *, void *))streq);
  ASSERT_PASSED(0);

  return ASSERTION_PASSED;
}

assertion_result_t kind(void) {
  result = acrt_result_new_bool("", 0, 0, 0);
  ASSERT_KIND(BOOLEAN_ASSERTION_KIND);

  result = acrt_result_new_bool("", 1, (intptr_t)NULL, 1);
  ASSERT_KIND(BOOLEAN_ASSERTION_KIND);

  result = acrt_result_new_eq("", 1, NULL, NULL, NULL);
  ASSERT_KIND(EQ_ASSERTION_KIND);

  return ASSERTION_PASSED;
}

assertion_result_t warning() {
  int (*func)(void *, void *) = (int (*)(void *, void *))streq;
  result = acrt_result_new_bool("", 1, 1, 0);
  ASSERT_WARNING(!acrt_result_is_warning(&result));

  result = acrt_result_new_bool("", 1, (intptr_t)"string", 1);
  ASSERT_WARNING(!acrt_result_is_warning(&result));

  result = acrt_result_new_eq("", 1, "string a", "string b", func);
  ASSERT_WARNING(!acrt_result_is_warning(&result));

  result = acrt_result_new_eq("", 1, "string a", "string b", NULL);
  ASSERT_WARNING(acrt_result_is_warning(&result) &&
                 result.data.eq_assertion.warning == EQ_FUNC_IS_NULL_WARNING);

  result = acrt_result_new_eq("", 1, "string", "string", func);
  ASSERT_WARNING(acrt_result_is_warning(&result) &&
                 result.data.eq_assertion.warning ==
                     SAME_ADDRESS_PARAMS_WARNING);

  result = acrt_result_new_eq("", 1, "string", "string", always_fail);
  ASSERT_WARNING(acrt_result_is_warning(&result) &&
                 result.data.eq_assertion.warning ==
                     SAME_ADDRESS_PARAMS_ARE_DIFF_WARNING);

  return ASSERTION_PASSED;
}

int always_fail(void *l, void *r) {
  // WARN: This is used to allow code compile since it uses -Wall -Werror
  //       -Wextra
  if (l == r)
    return 0;
  return 0;
}

int streq(char *s1, char *s2) {
  void *xor_ptr = (void *)((intptr_t)s1 ^ (intptr_t)s2);
  size_t len;

  if (xor_ptr || ((len = strlen(s1)) != strlen(s2)))
    return 0;

  for (size_t i = 0; i < len; i++) {
    if (s1[i] != s2[i])
      return 0;
  }

  return 1;
}
