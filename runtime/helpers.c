#include "helpers.h"

#include <string.h>

// Returns if the provided path string starts with the CURDIR_PREFIX (defined at helpers.h).
static inline bool starts_with_curdir_prefix(char *);

// Returns if the provided path string is an absolute path (by using ROOT_PREFIX from helpers.h).
static inline bool is_abspath(char *);

bool path_requires_curdir_prefix(char *path) {
  return (strlen(path) < strlen(CURDIR_PREFIX))
    || (!starts_with_curdir_prefix(path) && !is_abspath(path));
}

static inline bool starts_with_curdir_prefix(char *path) {
  return strncmp(path, CURDIR_PREFIX, strlen(CURDIR_PREFIX)) == 0;
}

static inline bool is_abspath(char *path) {
  return strncmp(path, ROOT_PREFIX, strlen(ROOT_PREFIX)) == 0;
}
