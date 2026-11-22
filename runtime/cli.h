#ifndef _ACRT_RUNTIME_CLI_H_
#define _ACRT_RUNTIME_CLI_H_

#include "helpers.h"

#include <stdbool.h>
#include <stdlib.h>

/* Data structure for the entire cli app. This is used by the parser and the runner
 * implementations and holds all required fields on pre-load:
 * - a pointer to the self executable path (const char *);
 * - flag codes being used (size_t);
 * - path counting (when dealing with inputs);
 * - inputs (paths). */
struct cli {

  size_t flags;
  int path_count;

  const char
    *self_path,
    *paths[ACRT_BUFFER_MAX_SIZE];
};

struct flag {
  const char *name, alias, *description;
  size_t code;
};

/* Returns if the flag alias matches with the provided char alias. */
bool flag_alias_matches(const struct flag *restrict, char);

/* Returns if the flag code matches with the provided size_t code. */
bool flag_code_matches(const struct flag *restrict, size_t);

/* Remove the second argument code bit from the first one. */
size_t flag_code_remove(size_t, size_t);

/* Returns if the flag long name matches with the provided string. */
bool flag_long_name_matches(const struct flag *restrict, const char *);
 
#endif
