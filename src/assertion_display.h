#ifndef _ASSERTION_DISPLAY_H_
#define _ASSERTION_DISPLAY_H_

#include "assertion_wrapper.h"

/* Different modes for assertion displaying. */
typedef enum {
  /* Display only error assertions. Keep succeeds silently. */
  DISPLAY_ERR_ONLY,
  /* Don't display assertion (even when err). */
  NO_DISPLAY,
  /* Display all assertions (even when ok). */
  DISPLAY_ALL_ASRT,
} assertion_display_mode_t;

/* Display the expression wrapper. */
void display_assertion_wrapper(assertion_wrapper_t *);

#endif
