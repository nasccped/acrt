#include <stdlib.h>
#include <string.h>

#include "acrt.h"

typedef struct __acrt_context global_context_t;
typedef struct __acrt_counting counting_t;
typedef struct __acrt_context_name context_name_t;
typedef struct __acrt_context_on_fail on_fail_t;

// Returns the context name when it somehow leads to NULL pointer.
#define CONTEXT_NAME_WHEN_NULL "NULL CONTEXT"

// Builds a __acrt_context_name struct with default values.
#define CONTEXT_NAME_BUILD_DEFAULT(FILE, FUNC)                                 \
  (context_name_t) {                                                           \
    .kind = CONTEXT_NAME_USE_FILE_NAME, .file = (FILE), .function = (FUNC),    \
    .custom = NULL                                                             \
  }

// Counting default state.
#define COUNTING_DEFAULT_STATE (counting_t){0}

// On fail default state.
#define ON_FAIL_DEFAULT_STATE                                                  \
  (on_fail_t){.kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,                     \
              .action.exit_code = 1}

// Default state for display mode.
#define DISPLAY_MODE_DEFAULT_STATE DISPLAY_MODE_FAILED_ONLY

// Takes the 'struct __acrt_context_on_fail' from the 'self' pointer.
#define UNWRAP_ON_FAIL(POINTER) &(POINTER)->__context.on_fail

acrt_t __acrt_default(const char *file_name, const char *function_name) {
  global_context_t context = {
      .name = CONTEXT_NAME_BUILD_DEFAULT(file_name, function_name),
      .counting = COUNTING_DEFAULT_STATE,
      .display_mode = DISPLAY_MODE_DEFAULT_STATE,
      .on_fail = ON_FAIL_DEFAULT_STATE};

  return (acrt_t){.__context = context};
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
