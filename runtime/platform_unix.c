#if defined(__linux__) || defined(__unix__)

#include "platform.h"

void plat_deinit(unix_struct_t *restrict us) {
  (void)us;
}

bool plat_init(unix_struct_t *restrict us, char *path) {
  (void)us;
  (void)path;

  // TODO: implement initializing.
  return true;
}

#endif
