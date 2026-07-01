#include "acrt.h"
#include <stdint.h>

/* Default state for the on fail context. */
static struct __acrt_context_on_fail ON_FAIL_DEFAULT_STATE = {
    .kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE, .action.exit_code = 1};

/* Default state for the conting field. */
static struct __acrt_counting COUNTING_DEFAULT_STATE = {0};

/* Returns the default __acrt_context_name struct. */
struct __acrt_context_name build_default_context_name(const char *,
                                                      const char *);

/* Macro used to avoid repetitive if (!pointer) return; */
#define IF_TRUE(EXPR, ACTION_BLOCK)                                            \
  if ((EXPR))                                                                  \
  ACTION_BLOCK

acrt_t __acrt_default(const char *file_name, const char *function_name) {
  struct __acrt_context context = {
      .name = build_default_context_name(file_name, function_name),
      .counting = COUNTING_DEFAULT_STATE,
      .display_mode = DISPLAY_MODE_FAILED_ONLY,
      .on_fail = ON_FAIL_DEFAULT_STATE};
  return (acrt_t){.__context = context};
}

void acrt_set_on_fail_exit_program(acrt_t *self, int code) {
  IF_TRUE(self, {
    self->__context.on_fail.kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE;
    self->__context.on_fail.action.exit_code = code;
  })
}

void acrt_set_on_fail_ignore_remaining_assertions(acrt_t *self) {
  IF_TRUE(self, {
    self->__context.on_fail.kind = ON_FAIL_IGNORE_REMAINING_ASSERTIONS;
  })
}

void acrt_set_on_fail_continue_remaining_assertions(acrt_t *self) {
  IF_TRUE(self, {
    self->__context.on_fail.kind = ON_FAIL_CONTINUE_REMAINING_ASSERTIONS;
  })
}

void acrt_set_on_fail_print_counting_and_exit_program(acrt_t *self, int code) {
  IF_TRUE(self, {
    struct __acrt_context_on_fail *fail = &self->__context.on_fail;
    struct __acrt_counting *counting = &self->__context.counting;
    fail->kind = ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE;
    fail->action.counting_and_exit.counting = counting;
    fail->action.counting_and_exit.exit_code = code;
  })
}

/* Run function callback and exit the program with the exit code. */
void acrt_set_on_fail_run_callback_and_exit_program(acrt_t *self,
                                                    void (*callback)(),
                                                    int code) {
  IF_TRUE(self, {
    self->__context.on_fail.kind = ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE;
    self->__context.on_fail.action.callback_and_exit.callback_function =
        callback;
    self->__context.on_fail.action.callback_and_exit.exit_code = code;
  })
}

void acrt_set_display_mode(acrt_t *self, acrt_display_mode_t mode) {
  IF_TRUE(self, self->__context.display_mode = mode);
}

void acrt_set_context_name_to_file(acrt_t *self) {
  IF_TRUE(self, self->__context.name.kind = CONTEXT_NAME_USE_FILE_NAME);
}

void acrt_set_context_name_to_function(acrt_t *self) {
  IF_TRUE(self, self->__context.name.kind = CONTEXT_NAME_USE_FUNCTION_NAME);
}

/* Set the acrt pointer context name to a custom char pointer. */
void acrt_set_context_name_to_custom(acrt_t *self, const char *name) {
  IF_TRUE(self && name, {
    self->__context.name.kind = CONTEXT_NAME_USE_CUSTOM_NAME;
    self->__context.name.name.custom = name;
  });
}

/* Returns the default __acrt_context_name struct. */
struct __acrt_context_name build_default_context_name(const char *file,
                                                      const char *function) {
  return (struct __acrt_context_name){
      .kind = CONTEXT_NAME_USE_FILE_NAME,
      .name = {.file = file, .function = function, .custom = NULL}};
}
