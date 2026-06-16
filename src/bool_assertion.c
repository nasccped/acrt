#include "bool_assertion.h"
#include <stdlib.h>
#include <string.h>

#define BOOL_ASSERTION_DEFAULT_NAME "BOOL_ASSERTION"

/* Default assert function for 'BoolAssertion'. Use
 * 'bool_assertion_new_with_function' or 'bool_assertion_set_function' for
 * custom asserting. */
int bool_assertion_default_function(int *data);

BoolAssertion *bool_assertion_new(void *data, size_t size) {
  // null data or zero size
  if (!data || !size)
    return NULL;
  BoolAssertion *ba;
  // alloc fails
  if (!(ba = malloc(sizeof(BoolAssertion))))
    return NULL;
  if (!(ba->data = malloc(size))) {
    free(ba);
    return NULL;
  }
  // set and copy mem
  ba->name = BOOL_ASSERTION_DEFAULT_NAME;
  ba->function = (BoolFunction)bool_assertion_default_function;
  memcpy_s(ba->data, size, data, size);
  return ba;
}

BoolAssertion *bool_assertion_new_with_function(void *data, size_t size,
                                                BoolFunction function) {
  BoolAssertion *ba;
  // if alloc pointer check/alloc fails
  if (!(ba = bool_assertion_new(data, size)))
    return NULL;
  // if func not null
  else if (function)
    ba->function = function;
  return ba;
}

void bool_assertion_destroy(BoolAssertion *self) {
  // self already null
  if (!self)
    return;
  // free deepcopy
  if (self->data)
    free(self->data);
  free(self);
}

void bool_assertion_set_function(BoolAssertion *self, BoolFunction function) {
  // self and function not null
  if (self && function)
    self->function = function;
}

int bool_assertion_run(BoolAssertion *self) {
  // if self/inner data not null: run function, else (negative).
  return (self && self->data && self->function) ? self->function(self->data)
                                                : 0;
}

void bool_assertion_set_name(BoolAssertion *self, const char *name) {
  // self is null
  if (!self)
    return;
  // name is null/zero length
  else if (!name || !strlen(name))
    self->name = BOOL_ASSERTION_DEFAULT_NAME;
  else
    self->name = name;
}

int bool_assertion_default_function(int *data) { return *data; }
