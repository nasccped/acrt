#include "assert_manager.h"
#include <stdlib.h>
#include <string.h>

AssertManager assert_manager_new() {
  return (AssertManager){.manager_name = ASSERT_MANAGER_DEFAULT_NAME};
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
