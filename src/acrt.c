#include "acrt.h"
#include "assertion_counter.h"
#include "assertion_display.h"
#include "assertion_wrapper.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defalt group name for the 'acrt_t' struct. */
#define DEFAULT_ACRT_NAME "UNAMMED ACRT"

/* If the current assertion should be run:
 * - pointer is not null
 * - previous wasn't fail or 'on_fail' is set to 'CONTINUE_ASSERTIONS' */
int __acrt_should_run(acrt_t *);

/* Updates a given acrt pointer based on a result integer. */
void __acrt_update(acrt_t *, int);

/* Exit the program with a default message if 'on_fail' is set to 'EXIT_PROGRAM'
 * and previous assertion was failed. */
void __acrt_fail_exit(acrt_t *);

void __acrt_run_bool_assertion(acrt_t *self, const char *file,
                               const unsigned int line, const char *expr,
                               const int result) {
  if (!__acrt_should_run(self))
    return;
  __acrt_update(self, result);
  switch (self->display_mode) {
  case NO_ASSERTION_DISPLAY:
    break;
  case DISPLAY_ERRORS_ONLY:
    if (result) {
      break;
    }
    __attribute__((fallthrough));
  case DISPLAY_ALL_ASSERTIONS:
    assertion_wrapper_t w =
        assertion_wrapper_new_bool(file, line, expr, self->name, result);
    display_assertion_wrapper(&w);
    break;
  }
  __acrt_fail_exit(self);
}

acrt_t acrt_new() {
  return (acrt_t){.name = DEFAULT_ACRT_NAME,
                  .counter = {0},
                  .on_fail = EXIT_PROGRAM,
                  .display_mode = DISPLAY_ERRORS_ONLY,
                  .previous = NO_PREVIOUS_ASSERTION};
}

void acrt_set_name(acrt_t *self, const char *name) {
  if (self && name && strlen(name))
    self->name = name;
}

void acrt_reset_counter(acrt_t *self) {
  if (self)
    assertion_counter_reset(&self->counter);
}

void acrt_set_display_variant(acrt_t *self, __acrt_display_t mode) {
  if (self)
    self->display_mode = mode;
}

void acrt_set_on_fail(acrt_t *self, __acrt_on_fail_t on_fail) {
  if (self)
    self->on_fail = on_fail;
}

int __acrt_should_run(acrt_t *self) {
  return (self) && (self->on_fail == CONTINUE_ASSERTIONS ||
                    self->previous != ASSERTION_WAS_FAILED);
}

void __acrt_update(acrt_t *self, int success) {
  if (!self)
    return;
  assertion_counter_update(&self->counter, success);
  self->previous = success ? ASSERTION_WAS_PASSED : ASSERTION_WAS_FAILED;
}

void __acrt_fail_exit(acrt_t *self) {
  if (!self) {
    fprintf(stderr, "\n'__acrt_fail_exit' function was called but 'acrt' "
                    "pointer is null.\n"
                    "exiting anyway...\n\n");
    fflush(stderr);
    exit(1);
  } else if (self->on_fail == EXIT_PROGRAM &&
             self->previous == ASSERTION_WAS_FAILED) {
    fprintf(stderr, "\n'acrt_t->on_fail' was set to 'EXIT_PROGRAM' "
                    "('acrt_set_on_fail' to change behavior).\n"
                    "exiting...\n\n");
    fflush(stderr);
    exit(1);
  }
}
