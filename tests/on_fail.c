#include "../src/acrt.h"
#include "test_util.h"

// Asserts the on fail callback argument pointer.
#define ASSERT_CALLBACK_ARG_POINTER(EXPECTED, GOT)                             \
  ASSERT_EQ_POINTER("OnFailCallbackArgPointer", (EXPECTED), (GOT))

// Asserts the on fail function callback pointer.
#define ASSERT_CALLBACK_EXIT_CODE(EXPECTED, GOT)                               \
  ASSERT_EQ_INT("OnFailCallbackExitCode", (EXPECTED), (GOT))

// Asserts the on fail function callback pointer.
#define ASSERT_CALLBACK_FUNCTION_POINTER(EXPECTED, GOT)                        \
  ASSERT_EQ_POINTER("OnFailCallbackFuncPointer", (EXPECTED), (GOT))

// Asserts on fail counting pointer.
#define ASSERT_COUNTING_EXIT_CODE(EXPECTED, GOT)                               \
  ASSERT_EQ_INT("OnFailCountingExitCode", (EXPECTED), (GOT))

// Asserts on fail counting pointer.
#define ASSERT_COUNTING_POINTER(EXPECTED, GOT)                                 \
  ASSERT_EQ_POINTER("OnFailCountingPointer", (EXPECTED), (GOT))

// Asserts on fail exit code.
#define ASSERT_EXIT_CODE(EXPECTED, GOT)                                        \
  ASSERT_EQ_INT("OnFailExitCode", (EXPECTED), (GOT))

// Assert on fail kind.
#define ASSERT_KIND(EXPECTED, GOT)                                             \
  ASSERT_EQ_INT("OnFailKind", (int)(EXPECTED), (int)(GOT))

// This function does nothing, it's just used as 'on_fail_callback' setter
// param.
void callback_example(char *string);

assertion_result_t exit_program_with_exit_code();
assertion_result_t continue_assertions();
assertion_result_t print_counting_and_exit();
assertion_result_t run_callback_and_exit();
assertion_result_t skip_future_assertions();

static acrt_t acrt;

int main(void) {
  acrt = ACRT_NEW();

  GENERATE_TEST_CASES(tests, CAST_TO_ASSERT_FUNCTION(continue_assertions),
                      CAST_TO_ASSERT_FUNCTION(exit_program_with_exit_code),
                      CAST_TO_ASSERT_FUNCTION(print_counting_and_exit),
                      CAST_TO_ASSERT_FUNCTION(run_callback_and_exit),
                      CAST_TO_ASSERT_FUNCTION(skip_future_assertions));
  RUN_TEST_CASES(tests, NULL);

  return 0;
}

assertion_result_t continue_assertions() {
  acrt_set_on_fail_continue_assertions(&acrt);
  ASSERT_KIND(ON_FAIL_CONTINUE_ASSERTIONS, acrt.on_fail.action_kind);

  return ASSERTION_PASSED;
}

assertion_result_t exit_program_with_exit_code() {
  acrt_set_on_fail_exit_program(&acrt, 2);
  ASSERT_KIND(ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE, acrt.on_fail.action_kind);
  ASSERT_EXIT_CODE(2, acrt.on_fail.data.code);

  return ASSERTION_PASSED;
}

assertion_result_t print_counting_and_exit() {
  acrt_set_on_fail_print_counting_and_exit(&acrt, 4);
  ASSERT_KIND(ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE,
              acrt.on_fail.action_kind);
  ASSERT_COUNTING_EXIT_CODE(4, acrt.on_fail.data.print_counting_and_exit.code);
  ASSERT_COUNTING_POINTER(&acrt.counting,
                          acrt.on_fail.data.print_counting_and_exit.counting);

  return ASSERTION_PASSED;
}

assertion_result_t run_callback_and_exit() {
  char *s = "string";
  void (*callback)(void *) = (void (*)(void *))callback_example;
  acrt_set_on_fail_run_callback_and_exit(&acrt, callback, s, 24);

  ASSERT_KIND(ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE,
              acrt.on_fail.action_kind);
  ASSERT_CALLBACK_FUNCTION_POINTER(
      (void (*)(void *))callback_example,
      acrt.on_fail.data.run_callback_and_exit.callback);
  ASSERT_CALLBACK_EXIT_CODE(24, acrt.on_fail.data.run_callback_and_exit.code);
  ASSERT_CALLBACK_ARG_POINTER(s, acrt.on_fail.data.run_callback_and_exit.arg);

  return ASSERTION_PASSED;
}

assertion_result_t skip_future_assertions() {
  acrt_set_on_fail_disable_assertions(&acrt);
  ASSERT_KIND(ON_FAIL_DISABLE_ASSERTIONS, acrt.on_fail.action_kind);

  return ASSERTION_PASSED;
}

void callback_example(char *string) {
  printf("Running callback with %s.\n", string);
}
