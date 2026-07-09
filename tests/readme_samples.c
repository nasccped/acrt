/* The purpose of this file is actually guarantee that the code samples at
 * README compile... */

#include "../src/acrt.h"
#include "test_util.h"

assertion_result_t ghost_function() {
  // WARN: This function is used to allow gcc compile since '-Wall' and
  //       '-Werror' are toggle on and 'code_location' at 'test_util.h' is
  //       unused.
  ASSERT_EQ_INT("AllowCompile", 0, 0);
  return ASSERTION_PASSED;
}

assertion_result_t test_01();

int main() {
  GENERATE_TEST_CASES(tests, CAST_TO_ASSERT_FUNCTION(test_01));
  RUN_TEST_CASES(tests, NULL);
  return 0;
}

assertion_result_t test_01() {
  acrt_t acrt = ACRT_NEW();

  ACRT_BOOL(&acrt, 2 > 1);
  ACRT_BOOL(&acrt, 0.1 + 0.2 != 0.3);

  char *s = "hello";
  ACRT_BOOL(&acrt, s);
  s = NULL;
  ACRT_BOOL(&acrt, !s);

  return ASSERTION_PASSED;
}
