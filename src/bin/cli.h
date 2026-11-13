#ifndef _ACRT_CLI_H_
#define _ACRT_CLI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Max number of items that the program can handle without requiring reallocations.
#define BUFFER_MAX_SIZE 64

// Globally accessible program description (used at --help invoke).
#define PROGRAM_DESCRIPTION "Easy and ergonomic testing runtime for C apps"

// Use opaque type-definition to flag code identifier (defined at cli_flags.h).
typedef size_t fci_t;

typedef struct {
  // Path to the self executable.
  char *self_path;

  // Global flags being used (code identifier only).
  fci_t global_flags;

  // Paths to be ran.
  int path_count;
  char *paths[BUFFER_MAX_SIZE];

} cli_t;

// Does the cli parsing and returns a boolean meaning:
// - 'true' as succeeded;
// - 'false' as failed.
//
// The parsing error is reported at function-scoped runtime. Basically, out-scope function printing
// isn't necessary.
bool parse_cli(cli_t *cli, int argc, char *argv[]);

// Does the app running and returns a boolean where:
// - 'true' means run succeeded;
// - 'false' means run failed.
bool run_app(cli_t *cli);

#endif
