#ifndef _ACRT_RUNTIME_PARSER_H_
#define _ACRT_RUNTIME_PARSER_H_

#include "helpers.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct {

  // Path to the self executable.
  const char *self_path;

  // size_t code identifiers set referring to the cli flags being used.
  size_t flags;

  // Counting of passed paths.
  int path_count;

  // Passed paths.
  const char *paths[ACRT_BUFFER_MAX_SIZE];

} cli_t;

/* Does the stdin args parsing directly into the cli struct pointer. Reports to stderr if some
 * error occurs + returns false. */
bool cli_parse(cli_t *restrict, int, const char **);

#endif
