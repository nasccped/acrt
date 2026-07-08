#include <stdlib.h>
#include <string.h>

#include "acrt.h"
#include "acrt_display.h"
#include "acrt_result.h"

typedef struct __acrt_counting counting_t;

// Runs the acrt pointer early checks (update fields + return before actually
// asserting, if necessary).
#define EARLY_CHECKS(SELF)                                                     \
  do {                                                                         \
    if (!(SELF))                                                               \
      return 0;                                                                \
                                                                               \
    int do_not_run =                                                           \
        GET_ON_FAIL((SELF)).action_kind == ON_FAIL_DISABLE_ASSERTIONS;         \
    do_not_run |= GET_ON_FAIL((SELF)).action_kind ==                           \
                      ON_FAIL_RUN_CALLBACK_AND_DO_NOT_EXIT &&                  \
                  !GET_ON_FAIL((SELF))                                         \
                       .data.run_callback_and_do_not_exit.continue_assertions; \
    do_not_run &= (SELF)->previous_assertion_failed;                           \
    if (do_not_run) {                                                          \
      update_counting(&(SELF)->counting, &IGNORED_ASSERTION_RESULT);           \
      return 0;                                                                \
    }                                                                          \
  } while (0)

// Unwraps the context name.
#define GET_CTX_NAME(ACRT) (ACRT)->context_name

// Unwraps the display mode.
#define GET_DISPLAY(ACRT) (ACRT)->display_mode

// Alias for getting the acrt.on_fail field.
#define GET_ON_FAIL(ACRT) (ACRT)->on_fail

// Returns the self string pointer if not null, otherwise, a context name string
// placeholder.
#define STR_OR_CTX_NAME_PLACEHOLDER(PTR)                                       \
  (PTR) ? (PTR) : "UNDEFINED CONTEXT NAME"

// Handles the result stuff based on self inner state. Returns the int
// (passed, failed or ignored) from the result itself.
int acrt_handle_result(acrt_t *self, acrt_result_t *result);

// Performs the on_fail field action.
void acrt_perform_on_fail(acrt_t *self);

// If the current assertion should be displayed.
int acrt_should_display(acrt_t *self, acrt_result_t *result);

// Returns the acrt context name as string.
const char *context_name_as_str(acrt_t *self);

// Updates the inner data of counting pointer based on a acrt result kind.
void update_counting(counting_t *self, acrt_result_t *result);

acrt_t __acrt_default(const char *file_name, const char *function_name) {
  return (acrt_t){
      .context_name = {.kind = CONTEXT_NAME_USE_FILE_NAME,
                       .data = {.file = file_name,
                                .function = function_name,
                                .custom = STR_OR_CTX_NAME_PLACEHOLDER(NULL)}},
      .counting = (counting_t){0},
      .display_mode = DISPLAY_MODE_FAILED_ONLY,
      .on_fail = {.action_kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,
                  .data.code = 1},
      .previous_assertion_failed = 0,
  };
}

int __acrt_run_boolean_assertion_from_number(acrt_t *self,
                                             const unsigned int line,
                                             int number) {
  EARLY_CHECKS(self);

  acrt_result_t temp =
      acrt_result_from_int(context_name_as_str(self), line, number);

  return acrt_handle_result(self, &temp);
}

int __acrt_run_boolean_assertion_from_pointer(acrt_t *self,
                                              const unsigned int line,
                                              void *pointer) {
  EARLY_CHECKS(self);

  acrt_result_t temp =
      acrt_result_from_single_pointer(context_name_as_str(self), line, pointer);

  return acrt_handle_result(self, &temp);
}

void acrt_display_counting(acrt_t *self, FILE *f) {
  if (!self || !f)
    return;

  display_counting_data(context_name_as_str(self), &self->counting, f);
}

void acrt_set_on_fail_continue_assertions(acrt_t *self) {
  if (!self)
    return;

  GET_ON_FAIL(self).action_kind = ON_FAIL_CONTINUE_ASSERTIONS;
}

void acrt_set_on_fail_disable_assertions(acrt_t *self) {
  if (!self)
    return;

  GET_ON_FAIL(self).action_kind = ON_FAIL_DISABLE_ASSERTIONS;
}

void acrt_set_on_fail_exit_program(acrt_t *self, int code) {
  if (!self)
    return;

  GET_ON_FAIL(self).action_kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE;
  GET_ON_FAIL(self).data.code = code;
}

void acrt_set_on_fail_print_counting_and_exit(acrt_t *self, int code) {
  if (!self)
    return;

  GET_ON_FAIL(self).action_kind =
      ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE;
  GET_ON_FAIL(self).data.print_counting_and_exit.counting = &self->counting;
  GET_ON_FAIL(self).data.print_counting_and_exit.code = code;
}

void acrt_set_on_fail_run_callback_and_exit(acrt_t *self,
                                            void (*callback)(void *), void *arg,
                                            int code) {
  if (!self || !callback)
    return;

  GET_ON_FAIL(self).action_kind = ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE;
  GET_ON_FAIL(self).data.run_callback_and_exit.callback = callback;
  GET_ON_FAIL(self).data.run_callback_and_exit.arg = arg;
  GET_ON_FAIL(self).data.run_callback_and_exit.code = code;
}

void acrt_set_on_fail_run_callback_without_exit(acrt_t *self,
                                                void (*callback)(void *),
                                                void *arg,
                                                int continue_assertions) {
  if (!self || !callback)
    return;

  GET_ON_FAIL(self).action_kind = ON_FAIL_RUN_CALLBACK_AND_DO_NOT_EXIT;
  GET_ON_FAIL(self).data.run_callback_and_do_not_exit.callback = callback;
  GET_ON_FAIL(self).data.run_callback_and_do_not_exit.arg = arg;
  GET_ON_FAIL(self).data.run_callback_and_do_not_exit.continue_assertions =
      continue_assertions;
}

void acrt_reset_assertions_state(acrt_t *self) {
  if (!self)
    return;

  self->counting = (counting_t){0};
  self->previous_assertion_failed = 0;
}

// Set the acrt display mode field to 'DISPLAY_MODE_ALL' variant.
void acrt_set_display_mode_to_all(acrt_t *self) {
  if (!self)
    return;

  GET_DISPLAY(self) = DISPLAY_MODE_ALL;
}

// Set the acrt display mode field to 'DISPLAY_MODE_FAILED_ONLY' variant.
void acrt_set_display_mode_to_failed_only(acrt_t *self) {
  if (!self)
    return;

  GET_DISPLAY(self) = DISPLAY_MODE_FAILED_ONLY;
}

// Set the acrt display mode field to 'DISPLAY_MODE_QUIET' variant.
void acrt_set_display_mode_to_quiet(acrt_t *self) {
  if (!self)
    return;

  GET_DISPLAY(self) = DISPLAY_MODE_QUIET;
}

void acrt_set_context_name_to_custom(acrt_t *self, const char *name) {
  if (!self || !name || !strlen(name))
    return;

  GET_CTX_NAME(self).kind = CONTEXT_NAME_USE_CUSTOM_NAME;
  GET_CTX_NAME(self).data.custom = name;
}

void acrt_set_context_name_to_file(acrt_t *self) {
  if (!self)
    return;

  GET_CTX_NAME(self).kind = CONTEXT_NAME_USE_FILE_NAME;
}

void acrt_set_context_name_to_function(acrt_t *self) {
  if (!self)
    return;

  GET_CTX_NAME(self).kind = CONTEXT_NAME_USE_FUNCTION_NAME;
}

const char *context_name_as_str(acrt_t *self) {
  if (!self)
    return NULL;

  switch (GET_CTX_NAME(self).kind) {
  case CONTEXT_NAME_USE_FILE_NAME:
    return STR_OR_CTX_NAME_PLACEHOLDER(GET_CTX_NAME(self).data.file);
  case CONTEXT_NAME_USE_FUNCTION_NAME:
    return STR_OR_CTX_NAME_PLACEHOLDER(GET_CTX_NAME(self).data.function);
  case CONTEXT_NAME_USE_CUSTOM_NAME:
    return STR_OR_CTX_NAME_PLACEHOLDER(GET_CTX_NAME(self).data.custom);
  }

  return STR_OR_CTX_NAME_PLACEHOLDER(NULL);
}

int acrt_handle_result(acrt_t *self, acrt_result_t *result) {
  if (acrt_should_display(self, result))
    display_acrt_result(result);

  update_counting(&self->counting, result);

  if (!acrt_result_is_passed(result))
    acrt_perform_on_fail(self);

  return acrt_result_is_passed(result);
}

int acrt_should_display(acrt_t *self, acrt_result_t *result) {
  if ((!self || !result) || (GET_DISPLAY(self) == DISPLAY_MODE_QUIET) ||
      (GET_DISPLAY(self) == DISPLAY_MODE_FAILED_ONLY &&
       result->status != FAILED_ASSERTION))
    return 0;
  else
    return 1;
}

void acrt_perform_on_fail(acrt_t *self) {
  if (!self)
    return;

  void (*callback)(void *), *arg;
  int code;
  counting_t *counting;

  switch (self->on_fail.action_kind) {
  case ON_FAIL_DISABLE_ASSERTIONS:
    self->previous_assertion_failed = 1;
  case ON_FAIL_CONTINUE_ASSERTIONS:
    break;

  case ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE:
    code = GET_ON_FAIL(self).data.print_counting_and_exit.code;
    counting = GET_ON_FAIL(self).data.print_counting_and_exit.counting;
    display_counting_data(context_name_as_str(self), counting, stderr);
    exit(code);
    break;

  case ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE:
    code = GET_ON_FAIL(self).data.code;
    exit(code);
    break;

  case ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE:
    callback = GET_ON_FAIL(self).data.run_callback_and_exit.callback;
    arg = GET_ON_FAIL(self).data.run_callback_and_exit.arg;
    code = GET_ON_FAIL(self).data.run_callback_and_exit.code;
    callback(arg);
    exit(code);
    break;

  case ON_FAIL_RUN_CALLBACK_AND_DO_NOT_EXIT:
    callback = self->on_fail.data.run_callback_and_do_not_exit.callback;
    arg = GET_ON_FAIL(self).data.run_callback_and_do_not_exit.arg;
    callback(arg);
    break;
  }
}

void update_counting(counting_t *self, acrt_result_t *result) {
  if (!self)
    return;

  switch (result->status) {
  case PASSED_ASSERTION_WITHOUT_WARNING:
    self->passed.without_warnings++;
    break;
  case PASSED_ASSERTION_WITH_WARNING:
    self->passed.with_warnings++;
    break;
  case FAILED_ASSERTION:
    self->failed++;
    break;
  case IGNORED_ASSERTION:
    self->ignored++;
    break;
  }

  self->total++;
}
