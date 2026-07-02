#include <stdio.h>
#include <string.h>

#include "../src/acrt.h"

#define KIND_TO_STRING(KIND)                                                   \
  (KIND) == CONTEXT_NAME_USE_FILE_NAME ? "CONTEXT_NAME_USE_FILE_NAME"          \
  : (KIND) == CONTEXT_NAME_USE_FUNCTION_NAME                                   \
      ? "CONTEXT_NAME_USE_FUNCTION_NAME"                                       \
      : "CONTEXT_NAME_USE_CUSTOM_NAME"

static acrt_name_kind_t *name_kind;
static int name_kind_assertion_counter = 1;

#define TO_VOID_POINTER(ACRT) (void *)(ACRT).__context.name.custom

// Does the name kind assertion and returns its result as integer.
int assert_name_kind(acrt_name_kind_t expected);

// Does the file name field assertion and returns its result as integer.
int assert_eq_str(const char *expected, const char *got);

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

int assert_name_kind(acrt_name_kind_t expected) {
  int result = 1;
  if (expected != *name_kind) {
    fprintf(stderr, "Name kind assertion %d failed:\n expecting %s\n got %s\n",
            name_kind_assertion_counter, KIND_TO_STRING(expected),
            KIND_TO_STRING(*name_kind));
    result = 0;
  } else {
    printf("Name kind assertion %d passed!\n", name_kind_assertion_counter);
  }
  name_kind_assertion_counter++;
  return result;
}

int assert_eq_str(const char *expected, const char *got) {
  int result = 1;
  if (strcmp(expected, got) != 0) {
    fprintf(stderr, "String eq assertion failed:\n expecting %s\n got %s\n",
            expected, got);
    result = 0;
  }
  return result;
}

int assert_pointer(void *p, int expecting_null) {
  int result = (p == NULL) == expecting_null;
  if (!result) {
    if (p == NULL)
      fprintf(stderr, "Passed pointer was expected to be not null.\n");
    else
      fprintf(stderr,
              "Passed pointer was expected to be null but it points to %p.\n",
              p);
    result = 0;
  }
  return result;
}
