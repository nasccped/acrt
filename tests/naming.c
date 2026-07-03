#include <stdio.h>
#include <string.h>

#include "../src/acrt.h"
#include "test_util.h"

#define KIND_TO_STRING(KIND)                                                   \
  (KIND) == CONTEXT_NAME_USE_FILE_NAME ? "CONTEXT_NAME_USE_FILE_NAME"          \
  : (KIND) == CONTEXT_NAME_USE_FUNCTION_NAME                                   \
      ? "CONTEXT_NAME_USE_FUNCTION_NAME"                                       \
      : "CONTEXT_NAME_USE_CUSTOM_NAME"

static acrt_name_kind_t *name_kind;

#define TO_VOID_POINTER(ACRT) (void *)(ACRT).__context.name.custom

// Does the file name field assertion and returns its result as integer.
int assert_eq_str(const char *expected, const char *got);

// Does the name kind assertion and returns its result as integer.
int assert_name_kind(acrt_name_kind_t expected);

// Asserts that the passed string points to NULL.
int assert_pointer(void *p, int expecting_null);

int main(void) {
  acrt_t acrt = ACRT_NEW();
  name_kind = &acrt.__context.name.kind;

  // assert file name
  if (!assert_name_kind(CONTEXT_NAME_USE_FILE_NAME))
    return 1;
  else if (!assert_eq_str(__FILE__, acrt.__context.name.file))
    return 1;
  else if (!assert_pointer(TO_VOID_POINTER(acrt), 1))
    return 1;

  acrt_set_name_to_function(&acrt);

  // assert function name.
  if (!assert_name_kind(CONTEXT_NAME_USE_FUNCTION_NAME))
    return 1;
  else if (!assert_eq_str(__FUNCTION__, acrt.__context.name.function))
    return 1;
  else if (!assert_pointer(TO_VOID_POINTER(acrt), 1))
    return 1;

  acrt_set_name_to_custom(&acrt, "cool");

  // assert custom name.
  if (!assert_name_kind(CONTEXT_NAME_USE_CUSTOM_NAME))
    return 1;
  else if (!assert_eq_str("cool", acrt.__context.name.custom))
    return 1;
  // if is null, actually...
  else if (!assert_pointer(TO_VOID_POINTER(acrt), 0))
    return 1;

  return 0;
}

int assert_eq_str(const char *expected, const char *got) {
  ASSERT_EQ_STRING("StringEq", expected, got);
  return ASSERTION_PASSED;
}

int assert_name_kind(acrt_name_kind_t expected) {
  ASSERT_EQ_WITH_STRING_FUNC("Name kind", expected, *name_kind, KIND_TO_STRING);
  return ASSERTION_PASSED;
}

int assert_pointer(void *p, int expecting_null) {
  ASSERT_EQ_POINTER("Pointer", p, expecting_null ? NULL : p);
  return ASSERTION_PASSED;
}
