#include "../src/acrt.h"
#include "test_util.h"

// Asserts if two custom names are the same (strings).
#define ASSERT_CUSTOM_NAME(EXPECTED, GOT)                                      \
  ASSERT_EQ_STRING("CustomName", (EXPECTED), (GOT))

// Asserts if expected file name is the same as the got one (strings).
#define ASSERT_FILE_NAME(EXPECTED, GOT)                                        \
  ASSERT_EQ_STRING("FileName", (EXPECTED), (GOT))

// Asserts if expected function name is the same as the got one (strings).
#define ASSERT_FUNCTION_NAME(EXPECTED, GOT)                                    \
  ASSERT_EQ_STRING("FunctionName", (EXPECTED), (GOT))

// Asserts if the expected name kind refers to the same variant of got one.
#define ASSERT_NAME_KIND(EXPECTED, GOT)                                        \
  ASSERT_EQ_INT("NameKind", (int)(EXPECTED), (int)(GOT))

static acrt_t acrt;

assertion_result_t custom();
assertion_result_t file_name();
assertion_result_t function_name();
assertion_result_t name_kind();

int main(void) {
  GENERATE_TEST_CASES(tests, CAST_TO_ASSERT_FUNCTION(custom),
                      CAST_TO_ASSERT_FUNCTION(file_name),
                      CAST_TO_ASSERT_FUNCTION(function_name),
                      CAST_TO_ASSERT_FUNCTION(name_kind));
  RUN_TEST_CASES(tests, NULL);

  return 0;
}

assertion_result_t custom() {
  acrt = ACRT_NEW();
  acrt_set_context_name_to_custom(&acrt, "some test");
  ASSERT_CUSTOM_NAME("some test", acrt.context_name.data.custom);

  acrt_set_context_name_to_custom(&acrt, "abc");
  ASSERT_CUSTOM_NAME("abc", acrt.context_name.data.custom);

  return ASSERTION_PASSED;
}

assertion_result_t file_name() {
  acrt = ACRT_NEW();
  ASSERT_FILE_NAME(__FILE__, acrt.context_name.data.file);
  return ASSERTION_PASSED;
}

assertion_result_t function_name() {
  acrt = ACRT_NEW();
  // WARN: hardcoded assertion.
  ASSERT_FUNCTION_NAME("function_name", acrt.context_name.data.function);
  return ASSERTION_PASSED;
}

assertion_result_t name_kind() {
  acrt = ACRT_NEW();
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_FILE_NAME, acrt.context_name.kind);

  acrt_set_context_name_to_function(&acrt);
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_FUNCTION_NAME, acrt.context_name.kind);

  acrt_set_context_name_to_custom(&acrt, "custom");
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_CUSTOM_NAME, acrt.context_name.kind);

  acrt_set_context_name_to_file(&acrt);
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_FILE_NAME, acrt.context_name.kind);

  return ASSERTION_PASSED;
}
