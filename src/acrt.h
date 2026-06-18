#ifndef _ACRT_H_
#define _ACRT_H_

#include "assertion_counter.h"

/* Display modes when running an 'acrt_t'. */
typedef enum {
  /* Display only on error assertions. */
  DISPLAY_ERRORS_ONLY,
  /* Display on all assertions (success included). */
  DISPLAY_ALL_ASSERTIONS,
  /* Don't display assertion (even when error). */
  NO_ASSERTION_DISPLAY,
} __acrt_display_t;

/* What to do on assertion fails. */
typedef enum {
  /* Exit program with error code (1). */
  EXIT_PROGRAM,
  /* Continue program and skip all next assertions. */
  SKIP_ASSERTIONS,
  /* Continue program and all other assertions normally. */
  CONTINUE_ASSERTIONS,
} __acrt_on_fail_t;

/* Holds the previous assertion state (useful when 'SKIP_ASSERTIONS' is on). */
typedef enum {
  /* When there's no previous assertion. */
  NO_PREVIOUS_ASSERTION,
  /* When the previous assertion was passed. */
  ASSERTION_WAS_PASSED,
  /* When the previous assertion was failed. */
  ASSERTION_WAS_FAILED,
} __acrt_previous_t;

/* Default asserter manager. */
typedef struct {
  /* Name for the current acrt group. */
  const char *name;
  /* What to do on fail assertions. */
  __acrt_on_fail_t on_fail;
  /* Display mode. */
  __acrt_display_t display_mode;
  /* Previous assertion state. */
  __acrt_previous_t previous;
  /* Assertion counting. */
  assertion_counter_t counter;
} acrt_t;

/* Runs the assertion as boolean expression. */
void __acrt_run_bool_assertion(acrt_t *, const char *, const unsigned int,
                               const char *, const int);

/* Runs a boolean assertion. */
#define acrt_bool(POINTER, EXPR)                                               \
  __acrt_run_bool_assertion(&(POINTER), __FILE__, __LINE__, #EXPR, (!!(EXPR)))

/* Creates a new acrt struct. The name and counter is set to default. You can
 * still set a custom name by using 'acrt_set_name' function. */
acrt_t acrt_new();

/* Set a custom name to an already existing 'acrt' struct. Fails if self/name is
 * null or name is zero lenght string. */
void acrt_set_name(acrt_t *, const char *);

/* Reset the counter inner values. */
void acrt_reset_counter(acrt_t *);

/* Set's a new 'display_variant_t' for the self acrt struct. */
void acrt_set_display_variant(acrt_t *, __acrt_display_t);

/* Set's a new 'on_fail' action to the self 'acrt'. */
void acrt_set_on_fail(acrt_t *, __acrt_on_fail_t);

#endif
