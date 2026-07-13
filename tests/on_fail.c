#include "../src/acrt.h"
#include "test_util.h"

// Asserts if the argument field points to the same address as the EXPECTED
// param.
//
// This macro requires an 'SHOULD_EXIT' bool used to decide wich field to
// unwraps the arg ('run_callback_and_exit' or 'run_callback_and_do_not_exit').
#define ASSERT_CALLBACK_ARG_POINTER(EXPECTED, SHOULD_EXIT)                     \
  ASSERT_EQ_POINTER("OnFailCallbackArgPointer", (EXPECTED),                    \
                    (SHOULD_EXIT)                                              \
                        ? ON_FAIL_DATA(acrt).run_callback_and_exit.arg         \
                        : ON_FAIL_DATA(acrt).run_callback_and_do_not_exit.arg)

// Asserts if the code field within a callback option is the same as the
// EXPECTED param.
//
// This macro requires an 'SHOULD_EXIT' bool used to decide wich field to
// unwraps the arg ('run_callback_and_exit' or 'run_callback_and_do_not_exit').
#define ASSERT_CALLBACK_EXIT_CODE(EXPECTED)                                    \
  ASSERT_EQ_INT("OnFailCallbackExitCode", (EXPECTED),                          \
                ON_FAIL_DATA(acrt).run_callback_and_exit.code)

// Asserts if the callback field points to the same address as the EXPECTED
// param.
//
// This macro requires an 'SHOULD_EXIT' bool used to decide wich field to
// unwraps the arg ('run_callback_and_exit' or 'run_callback_and_do_not_exit').
#define ASSERT_CALLBACK_FUNCTION_POINTER(EXPECTED, SHOULD_EXIT)                \
  ASSERT_EQ_POINTER(                                                           \
      "OnFailCallbackFuncPointer", (EXPECTED),                                 \
      (SHOULD_EXIT)                                                            \
          ? ON_FAIL_DATA(acrt).run_callback_and_exit.callback                  \
          : ON_FAIL_DATA(acrt).run_callback_and_do_not_exit.callback)

#define ASSERT_COUNTING_EXIT_CODE(EXPECTED)                                    \
  ASSERT_EQ_INT("OnFailCountingExitCode", (EXPECTED),                          \
                ON_FAIL_DATA(acrt).print_counting_and_exit.code)

#define ASSERT_COUNTING_POINTER(EXPECTED)                                      \
  ASSERT_EQ_POINTER("OnFailCountingPointer", (EXPECTED),                       \
                    ON_FAIL_DATA(acrt).print_counting_and_exit.counting)

// Asserts on fail exit code.
#define ASSERT_EXIT_CODE(EXPECTED)                                             \
  ASSERT_EQ_INT("OnFailExitCode", (EXPECTED), ON_FAIL_DATA(acrt).code)

#define ASSERT_KIND(EXPECTED)                                                  \
  ASSERT_EQ_INT("OnFailKind", (int)(EXPECTED), (int)acrt.on_fail.action_kind)

// Returns the on fail field from an acrt struct.
#define ON_FAIL_DATA(ACRT) (ACRT).on_fail.data

#define SHOULD_EXIT 1
#define SHOULD_NOT_EXIT 0

// This function does nothing, it's just used as 'on_fail_callback' setter
// param.
void callback_example(char *string);

assertion_result_t exit_program_with_exit_code();
assertion_result_t continue_assertions();
assertion_result_t print_counting_and_exit();
assertion_result_t run_callback_and_exit();
assertion_result_t run_callback_and_do_not_exit();
assertion_result_t skip_future_assertions();

static acrt_t acrt;

int main(void) {
  acrt = ACRT_DEFAULT;

  GENERATE_TEST_CASES(tests, CAST_TO_ASSERT_FUNCTION(continue_assertions),
                      CAST_TO_ASSERT_FUNCTION(exit_program_with_exit_code),
                      CAST_TO_ASSERT_FUNCTION(print_counting_and_exit),
                      CAST_TO_ASSERT_FUNCTION(run_callback_and_exit),
                      CAST_TO_ASSERT_FUNCTION(run_callback_and_do_not_exit),
                      CAST_TO_ASSERT_FUNCTION(skip_future_assertions));
  RUN_TEST_CASES(tests, NULL);

  return 0;
}

assertion_result_t continue_assertions() {
  acrt_set_on_fail_continue_assertions(&acrt);
  ASSERT_KIND(ON_FAIL_CONTINUE_ASSERTIONS);

  return ASSERTION_PASSED;
}

assertion_result_t exit_program_with_exit_code() {
  acrt_set_on_fail_exit_program(&acrt, 2);
  ASSERT_KIND(ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE);
  ASSERT_EXIT_CODE(2);

  return ASSERTION_PASSED;
}

assertion_result_t print_counting_and_exit() {
  acrt_set_on_fail_print_counting_and_exit(&acrt, 4);
  ASSERT_KIND(ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE);
  ASSERT_COUNTING_EXIT_CODE(4);
  ASSERT_COUNTING_POINTER(&acrt.counting);

  return ASSERTION_PASSED;
}

assertion_result_t run_callback_and_exit() {
  char *s = "string";
  void (*callback)(void *) = (void (*)(void *))callback_example;
  acrt_set_on_fail_run_callback_and_exit(&acrt, callback, s, 24);

  ASSERT_KIND(ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE);
  ASSERT_CALLBACK_FUNCTION_POINTER((void (*)(void *))callback_example,
                                   SHOULD_EXIT);
  ASSERT_CALLBACK_EXIT_CODE(24);
  ASSERT_CALLBACK_ARG_POINTER(s, SHOULD_EXIT);

  return ASSERTION_PASSED;
}

assertion_result_t run_callback_and_do_not_exit() {
  void *null_pointer = NULL;
  void (*callback)(void *) = (void (*)(void *))callback_example;
  acrt_set_on_fail_run_callback_without_exit(&acrt, callback, null_pointer, 1);
  return ASSERTION_PASSED;
}

assertion_result_t skip_future_assertions() {
  acrt_set_on_fail_disable_assertions(&acrt);
  ASSERT_KIND(ON_FAIL_DISABLE_ASSERTIONS);

  return ASSERTION_PASSED;
}

void callback_example(char *string) {
  printf("Running callback with %s.\n", string);
}
