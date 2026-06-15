#include "assert_manager.h"
#include <stdlib.h>
#include <string.h>

/* 'AssertManager' default naming. */
#define ASSERT_MANAGER_DEFAULT_NAME "<GENERIC ASSERTION>"

AssertManager *assert_manager_new(const char *name) {
  AssertManager *man = NULL;
  if ((man = malloc(sizeof(AssertManager)))) {
    // ternary op. naming
    man->manager_name =
        (name && strlen(name)) ? name : ASSERT_MANAGER_DEFAULT_NAME;
  }
  return man;
}

void assert_manager_destroy(AssertManager *self) {
  if (self)
    free(self);
}

AssertManager *assert_manager_set_name(AssertManager *self, char *name) {
  // if self is null
  if (!self)
    return NULL;
  // if name is not null + length greater than 0
  else if (name && strlen(name))
    self->manager_name = name;
  return self;
}
