#ifndef _ASSERTION_COUNTER_H_
#define _ASSERTION_COUNTER_H_

#include <stdint.h>

/* Assertion counting related struct. */
typedef struct {
  /* Total of assertions done. */
  uint8_t total;
  /* Total of passed assertions. */
  uint8_t passed;
  /* Total of failed assertions. */
  uint8_t failed;
} assertion_counter_t;

/* Reset the assertion counter to it's original state. */
void assertion_counter_reset(assertion_counter_t *);

/* Updates the self counter pointer based on an integer result. */
void assertion_counter_update(assertion_counter_t *, int);

#endif
