#include "platform.h"

#include <stdio.h>

bool platform_run(int path_count, const char **paths, size_t flags) {
  (void)path_count;
  (void)paths;
  (void)flags;

  // TODO:
  printf("Implement platform specifics!\n");
  return true;
}
