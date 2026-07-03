#include <stdio.h>
#include <string.h>

#include "../src/acrt.h"
#include "test_util.h"

#define ON_FAIL_KIND_TO_STR(KIND)                                              \
  (KIND) == ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE                                \
      ? "ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE"                                  \
  : (KIND) == ON_FAIL_SKIP_FUTURE_ASSERTIONS                                   \
      ? "ON_FAIL_SKIP_FUTURE_ASSERTIONS"                                       \
  : (KIND) == ON_FAIL_CONTINUE_ASSERTIONS ? "ON_FAIL_CONTINUE_ASSERTIONS"      \
  : (KIND) == ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE           \
      ? "ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE"               \
  : (KIND) == ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE                     \
      ? "ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE"                         \
      : "UNDEFINED INTEGER"

typedef struct __acrt_context_on_fail on_fail_t;
typedef struct __acrt_counting counting_t;
typedef enum __acrt_on_fail_kind kind_t;

// Asserts if the 'on_fail->action.callback_and_exit' fields fit to expected
// ones.
int assert_callback(void (*expected_func)(void *), void *expected_arg,
                    int expecte_code);

// Asserts if p points to the same address of counting pointer + code is the
// same as expected.
int assert_counting_and_exit(counting_t *expected_address, int expected_code);

// Asserts if the 'on_fail->action.exit_code' refers to the expected exit code.
int assert_exit_code(int expected_code);

// Asserts if the 'on_fail->kind' refers to the expected variant.
int assert_kind(kind_t expected_kind);

// This function does nothing, it's just used as 'on_fail_callback' setter
// param.
void callback_example(char *string);

static acrt_t acrt;
static on_fail_t *on_fail;

int main(void) {
  acrt = ACRT_NEW();
  on_fail = &acrt.__context.on_fail;

  if (!assert_kind(ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE))
    return 1;
  else if (!assert_exit_code(1))
    return 1;

  acrt_on_fail_continue_assertions(&acrt);

  if (!assert_kind(ON_FAIL_CONTINUE_ASSERTIONS))
    return 1;

  acrt_on_fail_print_counting_and_exit(&acrt, 3);

  if (!assert_kind(ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE))
    return 1;
  else if (!assert_counting_and_exit(&acrt.__context.counting, 3))
    return 1;

  acrt_on_fail_skip_assertions(&acrt);

  if (!assert_kind(ON_FAIL_SKIP_FUTURE_ASSERTIONS))
    return 1;

  char *string = "my string";
  void (*callback)(void *) = (void (*)(void *))callback_example;

  acrt_on_fail_run_callback_and_exit(&acrt, callback, string, 2);

  if (!assert_kind(ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE))
    return 1;
  else if (!assert_callback((void (*)(void *))callback_example, string, 2))
    return 1;

  // go back original state.
  acrt_on_fail_exit_program(&acrt, 42);

  if (!assert_kind(ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE))
    return 1;
  else if (!assert_exit_code(42))
    return 1;

  return 0;
}

int assert_callback(void (*expected_func)(void *), void *expected_arg,
                    int expecte_code) {
  ASSERT_EQ_POINTER("OnFailCallback", expected_func,
                    on_fail->action.callback_and_exit.callback_function);
  ASSERT_EQ_POINTER("OnFailArg", expected_arg,
                    on_fail->action.callback_and_exit.arg);
  ASSERT_EQ_INT("OnFailCallbackExitCode", expecte_code,
                on_fail->action.callback_and_exit.exit_code);
  return ASSERTION_PASSED;
}

int assert_counting_and_exit(counting_t *expected_address, int expected_code) {
  ASSERT_EQ_POINTER("CountingAddress", expected_address,
                    on_fail->action.counting_and_exit.counting);
  ASSERT_EQ_INT("CountingExitCode", expected_code,
                on_fail->action.counting_and_exit.exit_code);
  return ASSERTION_PASSED;
}

int assert_exit_code(int expected_code) {
  ASSERT_EQ_INT("ExitCode", expected_code, on_fail->action.exit_code);
  return ASSERTION_PASSED;
}

int assert_kind(kind_t expected_kind) {
  ASSERT_EQ_WITH_STRING_FUNC("OnFailKind", expected_kind, on_fail->kind,
                             ON_FAIL_KIND_TO_STR);
  return ASSERTION_PASSED;
}

void callback_example(char *string) {
  printf("Running callback with %s.\n", string);
}
