#include "../src/acrt.h"
#include "test_util.h"

// Asserts if two custom names are the same (strings).
#define ASSERT_CUSTOM_NAME(EXPECTED)                                           \
  ASSERT_EQ_STRING("CustomName", (EXPECTED), acrt.context_name.data.custom)

// Asserts if expected file name is the same as the got one (strings).
#define ASSERT_FILE_NAME(EXPECTED)                                             \
  ASSERT_EQ_STRING("FileName", (EXPECTED), acrt.context_name.data.file)

// Asserts if the expected name kind refers to the same variant of got one.
#define ASSERT_NAME_KIND(EXPECTED)                                             \
  ASSERT_EQ_INT("NameKind", (int)(EXPECTED), (int)acrt.context_name.kind)

static acrt_t acrt;

assertion_result_t custom();
assertion_result_t file_name();
assertion_result_t invalid();
assertion_result_t name_kind();

int main(void) {
  RUN_TESTS(custom, file_name, invalid, name_kind);
  return 0;
}

assertion_result_t custom() {
  acrt = ACRT_DEFAULT;
  acrt_set_context_name_to_custom(&acrt, "some test");
  ASSERT_CUSTOM_NAME("some test");

  acrt_set_context_name_to_custom(&acrt, "abc");
  ASSERT_CUSTOM_NAME("abc");

  return ASSERTION_PASSED;
}

assertion_result_t file_name() {
  acrt = ACRT_DEFAULT;
  ASSERT_FILE_NAME(__FILE__);
  return ASSERTION_PASSED;
}

assertion_result_t invalid() {
  acrt = ACRT_DEFAULT;

  acrt_set_context_name_to_custom(&acrt, NULL);
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_FILE_NAME);

  acrt_set_context_name_to_custom(&acrt, "");
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_FILE_NAME);

  return ASSERTION_PASSED;
}

assertion_result_t name_kind() {
  acrt = ACRT_DEFAULT;
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_FILE_NAME);

  acrt_set_context_name_to_custom(&acrt, "custom");
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_CUSTOM_NAME);

  acrt_set_context_name_to_file(&acrt);
  ASSERT_NAME_KIND(CONTEXT_NAME_USE_FILE_NAME);

  return ASSERTION_PASSED;
}
