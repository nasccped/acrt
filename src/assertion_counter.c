#include "assertion_counter.h"

void assertion_counter_reset(assertion_counter_t *self) {
  if (!self)
    return;
  self->total = 0;
  self->passed = 0;
  self->failed = 0;
}

void assertion_counter_update(assertion_counter_t *self, int success) {
  if (!self)
    return;
  if (success)
    self->passed += 1;
  else
    self->failed += 1;
  self->total += 1;
}
