#include "acrt.h"
#include "assertion_counter.h"
#include "assertion_display.h"
#include "assertion_wrapper.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defalt group name for the 'acrt_t' struct. */
#define DEFAULT_ACRT_NAME "UNNAMED ACRT"

/* Block that's ran when acrt should exit the program process. */
#define ACRT_DISPLAY_AND_EXIT()                                                \
  do {                                                                         \
    fprintf(stderr, "\n'acrt_t->on_fail' was set to 'EXIT_PROGRAM'.\n"         \
                    "exiting...\n\n");                                         \
    fflush(stderr);                                                            \
    exit(1);                                                                   \
  } while (0)

/* If the current assertion should be run:
 * - pointer is not null
 * - previous wasn't fail or 'on_fail' is set to 'CONTINUE_ASSERTIONS' */
int __acrt_should_run(acrt_t *);

/* If the current assertion should be displayed:
 * - pointer is not null
 * - display_mode is set to 'DISPLAY_ALL_ASSERTIONS' or ('DISPLAY_ERRORS_ONLY' +
 *   result is err) */
int __acrt_should_display(acrt_t *, int);

/* If the current state of acrt pointer requires proccess to be terminated. */
int __acrt_should_exit(acrt_t *);

/* Updates a given acrt pointer based on a result integer. */
void __acrt_update(acrt_t *, int);

int __acrt_run_bool_assertion(acrt_t *self, const char *file,
                              const unsigned int line, const char *expr,
                              const int result) {
  if (!__acrt_should_run(self))
    return result;
  __acrt_update(self, result);
  if (__acrt_should_display(self, result)) {
    assertion_wrapper_t w =
        assertion_wrapper_new_bool(file, line, expr, self->name, result);
    display_assertion_wrapper(&w);
  }
  if (__acrt_should_exit(self))
    ACRT_DISPLAY_AND_EXIT();
  return result;
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

int __acrt_should_display(acrt_t *self, int result) {
  return (self) && (self->display_mode == DISPLAY_ALL_ASSERTIONS ||
                    (self->display_mode == DISPLAY_ERRORS_ONLY && !result));
}

int __acrt_should_exit(acrt_t *self) {
  return (self) && (self->on_fail == EXIT_PROGRAM &&
                    self->previous == ASSERTION_WAS_FAILED);
}

void __acrt_update(acrt_t *self, int success) {
  if (!self)
    return;
  assertion_counter_update(&self->counter, success);
  self->previous = success ? ASSERTION_WAS_PASSED : ASSERTION_WAS_FAILED;
}
