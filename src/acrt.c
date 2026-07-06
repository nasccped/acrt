#include <stdlib.h>
#include <string.h>

#include "acrt.h"
#include "acrt_display.h"
#include "acrt_result.h"

typedef struct __acrt_context global_context_t;
typedef struct __acrt_context_name context_name_t;
typedef struct __acrt_context_on_fail on_fail_t;
typedef struct __acrt_counting counting_t;
typedef struct __acrt_result result_t;

// Runs the acrt pointer early checks (update fields + return before actually
// asserting, if necessary).
#define ACRT_RUN_EARLY_CHECKS(SELF)                                            \
  do {                                                                         \
    if (!(SELF))                                                               \
      return 0;                                                                \
                                                                               \
    if ((SELF)->__context.on_fail.kind == ON_FAIL_SKIP_FUTURE_ASSERTIONS &&    \
        (SELF)->__context.latest_was_failed) {                                 \
      (SELF)->__context.counting.total++;                                      \
      (SELF)->__context.counting.ignored++;                                    \
      return 0;                                                                \
    }                                                                          \
  } while (0)

// Returns the context name when it somehow leads to NULL pointer.
#define CONTEXT_NAME_OR_PLACEHOLDER(PTR) (PTR) ? (PTR) : "NULL CONTEXT"

// Builds a __acrt_context_name struct with default values.
#define CONTEXT_NAME_BUILD_DEFAULT(FILE, FUNC)                                 \
  (context_name_t) {                                                           \
    .kind = CONTEXT_NAME_USE_FILE_NAME, .file = (FILE), .function = (FUNC),    \
    .custom = NULL                                                             \
  }

// Counting default state.
#define COUNTING_DEFAULT_STATE (counting_t){0}

// Default state for display mode.
#define DISPLAY_MODE_DEFAULT_STATE DISPLAY_MODE_FAILED_ONLY

// On fail default state.
#define ON_FAIL_DEFAULT_STATE                                                  \
  (on_fail_t){.kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,                     \
              .action.exit_code = 1}

// Takes the 'struct __acrt_context_on_fail' from the 'self' pointer.
#define UNWRAP_ON_FAIL(POINTER) &(POINTER)->__context.on_fail

// Returns a properly string from a 'context_name_t' pointer.
#define STRING_FROM_CONTEXT_NAME(CTX)                                          \
  !(CTX) ? CONTEXT_NAME_OR_PLACEHOLDER(NULL)                                   \
  : (CTX)->kind == CONTEXT_NAME_USE_FILE_NAME                                  \
      ? CONTEXT_NAME_OR_PLACEHOLDER((CTX)->file)                               \
  : (CTX)->kind == CONTEXT_NAME_USE_FUNCTION_NAME                              \
      ? CONTEXT_NAME_OR_PLACEHOLDER((CTX)->function)                           \
  : (CTX)->kind == CONTEXT_NAME_USE_CUSTOM_NAME                                \
      ? CONTEXT_NAME_OR_PLACEHOLDER((CTX)->custom)                             \
      : CONTEXT_NAME_OR_PLACEHOLDER(NULL)

// Handles the result stuff based on self inner state. Returns the int (passed,
// failed or ignored) from the result itself.
int acrt_handle_result(acrt_t *self, result_t result);

acrt_t __acrt_default(const char *file_name, const char *function_name) {
  global_context_t context = {
      .name = CONTEXT_NAME_BUILD_DEFAULT(file_name, function_name),
      .counting = COUNTING_DEFAULT_STATE,
      .display_mode = DISPLAY_MODE_DEFAULT_STATE,
      .on_fail = ON_FAIL_DEFAULT_STATE};

  return (acrt_t){.__context = context};
}

int __acrt_run_boolean_assertion_from_number(acrt_t *self,
                                             const unsigned int line,
                                             int number) {
  ACRT_RUN_EARLY_CHECKS(self);

  const char *ctx_name = STRING_FROM_CONTEXT_NAME(&self->__context.name);
  result_t res = acrt_result_from_int(ctx_name, line, number);

  return acrt_handle_result(self, res);
}

int __acrt_run_boolean_assertion_from_pointer(acrt_t *self,
                                              const unsigned int line,
                                              void *pointer) {
  ACRT_RUN_EARLY_CHECKS(self);

  const char *ctx_name = STRING_FROM_CONTEXT_NAME(&self->__context.name);
  result_t res = acrt_result_from_single_pointer(ctx_name, line, pointer);

  return acrt_handle_result(self, res);
}

// Set a new display mode the self acrt pointer.
void acrt_display_mode(acrt_t *self, acrt_display_mode_t mode) {
  if (!self)
    return;

  self->__context.display_mode = mode;
}

void acrt_on_fail_continue_assertions(acrt_t *self) {
  if (!self)
    return;

  self->__context.on_fail.kind = ON_FAIL_CONTINUE_ASSERTIONS;
}

void acrt_on_fail_exit_program(acrt_t *self, int code) {
  if (!self)
    return;

  on_fail_t *on_fail = UNWRAP_ON_FAIL(self);

  on_fail->kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE;
  on_fail->action.exit_code = code;
}

void acrt_on_fail_print_counting_and_exit(acrt_t *self, int code) {
  if (!self)
    return;

  on_fail_t *on_fail = UNWRAP_ON_FAIL(self);

  on_fail->kind = ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE;
  on_fail->action.counting_and_exit.counting = &self->__context.counting;
  on_fail->action.counting_and_exit.exit_code = code;
}

void acrt_on_fail_run_callback_and_exit(acrt_t *self, void (*callback)(void *),
                                        void *arg, int code) {
  if (!self || !callback)
    return;

  on_fail_t *on_fail = UNWRAP_ON_FAIL(self);
  on_fail->kind = ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE;
  on_fail->action.callback_and_exit.callback_function = callback;
  on_fail->action.callback_and_exit.arg = arg;
  on_fail->action.callback_and_exit.exit_code = code;
}

void acrt_on_fail_skip_assertions(acrt_t *self) {
  if (!self)
    return;

  self->__context.on_fail.kind = ON_FAIL_SKIP_FUTURE_ASSERTIONS;
}

void acrt_reset_counting(acrt_t *self) {
  if (!self)
    return;

  self->__context.counting = (counting_t){0};
}

void acrt_set_name_to_custom(acrt_t *self, const char *name) {
  if (!self || !name || !strlen(name))
    return;

  context_name_t *cname = &self->__context.name;

  cname->kind = CONTEXT_NAME_USE_CUSTOM_NAME;
  cname->custom = name;
}

void acrt_set_name_to_file(acrt_t *self) {
  if (!self)
    return;

  self->__context.name.kind = CONTEXT_NAME_USE_FILE_NAME;
}

void acrt_set_name_to_function(acrt_t *self) {
  if (!self)
    return;

  self->__context.name.kind = CONTEXT_NAME_USE_FUNCTION_NAME;
}

int acrt_handle_result(acrt_t *self, result_t result) {
  acrt_display_mode_t *d_mode = &self->__context.display_mode;
  counting_t *counting = &self->__context.counting;

  int should_display = *d_mode == DISPLAY_MODE_ALL ||
                       (*d_mode == DISPLAY_MODE_FAILED_ONLY && !result.passed);

  if (should_display)
    display_acrt_result(&result);

  if (result.passed) {
    counting->passed += 1;
  } else {
    counting->failed += 1;
  }

  counting->total += 1;

  return result.passed;
}
