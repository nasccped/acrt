#ifndef _BOOL_ASSERTION_H_
#define _BOOL_ASSERTION_H_

#include <stdlib.h>

/* Function signature alias for 'BoolAssertion->function' field. */
typedef int (*BoolFunction)(void *);

/* Handles bool assertions. */
typedef struct {
  /* Name for the current assertion (NULL as default). */
  const char *name;
  /* Generic data being held. */
  void *data;
  /* Assert function to be performed with the 'data' field. */
  int (*function)(void *);
} BoolAssertion;

/* Creates a new 'BoolAssertion' with the provided data (void pointer) + it's
 * inner size. Note that the 'BoolAssertion' struct owns a deepcopy (memcpy_s)
 * of the provided data and changing the original value doesn't affect the inner
 * fields once the value was full copied.
 *
 * The 'name' field points to 'BOOL_ASSERTION_DEFAULT_NAME'. You can change it
 * using 'bool_assertion_set_name' function (the name must be const since it
 * isn't freed after usage).
 *
 * The 'function' field points to 'bool_assertion_default_function' which cast
 * the 'data' pointer to integer and do the boolean stuff. You can change it by
 * using the 'bool_assertion_new_with_function' func (does the same as this
 * function but pointing to a different function) or the
 * 'bool_assertion_set_function' (just set a new function to an already existing
 * 'BoolAssertion').
 *
 * Returns NULL either if data pointer is null/size is zero. */
BoolAssertion *bool_assertion_new(void *, size_t);

/* Does the same as 'bool_assertion_new' but changes the default function to a
 * custom one.
 *
 * Note that the fail conditions mentioned at 'bool_assertion_new' also occurs
 * on this one. */
BoolAssertion *bool_assertion_new_with_function(void *, size_t, BoolFunction);

/* Frees the self 'BoolAssertion' inner data + itself.
 *
 * The 'name' and 'function' field aren't freed since they're const like (at
 * stack). */
void bool_assertion_destroy(BoolAssertion *);

/* Changes the 'BoolAssertion->function' to the provided 'BoolFunction'. Does
 * nothing if self/BoolFunction is null. */
void bool_assertion_set_function(BoolAssertion *, BoolFunction);

/* Runs the 'self' BoolAssertion and returns its result as integer:
 * - '0' if fails (fails meaning: self/inner data is null | function run also
 *   returns 0)
 * - 'other' if succeeds */
int bool_assertion_run(BoolAssertion *);

/* Set a name to the self 'BoolAssertion' pointer. The 'name' field is expected
 * to be const since it isn't planned to be freed.
 *
 * Nothing is done if self is NULL + self->name is set to
 * 'BOOL_ASSERTION_DEFAULT_NAME' if provided name is NULL or zero length string.
 */
void bool_assertion_set_name(BoolAssertion *, const char *);

#endif
