#include "helpers.h"
#include "platform.h"
#include "runner.h"

#include <stdio.h>
#include <string.h>

// Tip shorthand when subcommand expects only verbose flag.
#define FLAG_CAN_ONLY_BE_USED_WITH_VERBOSE_ERR(FLAG_NAME) do { \
  TAGERR("'--%s' can only be used with '--%s'/'-%c' flag.", \
         (FLAG_NAME), FLN_VERBOSE, FSN_VERBOSE); \
} while (0)

#define MAX(L, R) ((L) > (R)) ? (L) : (R)

/* Data structure that maps a flag identifier (size_t) to a runner operation (function pointer). It
 * helps to avoid long if-else sequence blocks. */
typedef struct {

  // Id referring to the flag itself.
  size_t flag_id;

  // Runner for that flag in question.
  bool (*runner_fn)(cli_t *restrict);

} runner_map_t;

/* Runs the '--dump-version' kind of flag. This function also does other checks at cli fields
 * looking for conflicting state:
 * - it'll report the error to stderr and return false if so;
 * - otherwise, prints the program version (same as tag annotation) to stdout and returns true. */
static bool run_dump_version(cli_t *restrict);

/* Normal execution of acrt program (file entries). This function is inline just because it's a
 * wrapper for a platform dependent implementation. */
static inline bool run_files(cli_t *restrict);

/* Runs the '--help' kind of flag. All the extra checks are done the same as 'run_dump_version'
 * function. */
static bool run_help(cli_t *restrict);

/* Runs the '--version' kind of flag. All the extra checks are done the same as 'run_dump_version'
 * function. */
static bool run_version(cli_t *restrict);


/* Mapping of flags -> runner functions. Note that this function runner searching is done by linear
 * iteration. It means that top instance have greater priorities! */
static const runner_map_t RUNNER_MAPPINGS[] = {
  // flag code identifier     function pointer
  { FCI_HELP,                 run_help         },
  { FCI_DUMP_VERSION,         run_dump_version },
  { FCI_VERSION,              run_version      },
};

// Counting of RUNNER_MAPPINGS (helps on linear searching).
static const size_t RUNNER_MAPPING_COUNT = sizeof(RUNNER_MAPPINGS) / sizeof(runner_map_t);

bool run(cli_t *restrict cli) {
  // For each flag -> command convertible.
  for (size_t i = 0; i < RUNNER_MAPPING_COUNT; i++) {
    // If flag identifier matches.
    if (FLAG_INTERSECTION(cli->flags, RUNNER_MAPPINGS[i].flag_id)) {
      // Remove current flag id and pass operation responsibility to runner function.
      cli->flags = FLAG_DIFFERENCE(cli->flags, RUNNER_MAPPINGS[i].flag_id);
      return RUNNER_MAPPINGS[i].runner_fn(cli);
    }
  }

  // If no flag matches, do normal execution:
  return run_files(cli);
}

static bool run_dump_version(cli_t *restrict cli) {
  // If any other value than dump-version:
  if (cli->flags || cli->path_count) {
    TAGERR("'--%s' flag can't be used with other flags or path arguments.", FLN_DUMP_VERSION);
    return false;
  }

  // Otherwise, prints program tag:
  printf("%s\n", PROGRAM_TAG);
  return true;
}

static inline bool run_files(cli_t *restrict cli) {
  // If no files provided:
  if (!cli->path_count) {

    // If '--may-empty' flag missing:
    if (!FLAG_INTERSECTION(cli->flags, FCI_MAY_EMPTY)) {
      TAGERR("missing input file(s)/flag(s).");
      PRINTERR("Runs without files aren't allowed by default but can be forced\n"
               "with the '--%s' flag!\n\n", FLN_MAY_EMPTY);
      TAGTIP(stderr, "consider using '--%s' to get an usage overview!", FLN_HELP);
      return false;
    }

    // else:
    TAGWARN(stdout, "no files were passed.");
    printf("Exiting normally!\n");
    return true;
  }

  return platform_run(cli->path_count, cli->paths, cli->flags);
}

static bool run_help(cli_t *restrict cli) {
  // Flag description gap (fdg) between long name and it's description string.
  static size_t fdg = 0;

  // If any other value than verbose:
  if (FLAG_DIFFERENCE(cli->flags, FCI_VERBOSE) || cli->path_count) {
    FLAG_CAN_ONLY_BE_USED_WITH_VERBOSE_ERR(FLN_HELP);
    return false;
  }

  // Get greatest flag description gap:
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++)
    fdg = MAX(fdg, strlen(GLOBAL_FLAGS[i].long_name));

  // Increment on 3:
  fdg += 3;

  printf("%s.\n\n", PROGRAM_DESCRIPTION);
  printf("Usage: %s [OPTION(S)] [FILE_PATH(S)]\n\n", cli->self_path);
  printf("Options:\n");

  // Print each flag info:
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {
    printf("  ");

    if (GLOBAL_FLAGS[i].alias)
      printf("-%c, ", GLOBAL_FLAGS[i].alias);
    else
      printf("    ");

    printf("--%-*s %s\n",
           (int)fdg, GLOBAL_FLAGS[i].long_name, GLOBAL_FLAGS[i].description);
  }

  // If not verbose:
  if (!FLAG_INTERSECTION(cli->flags, FCI_VERBOSE)) {
    printf("\n");
    TAGTIP(stdout, "you can use '--%s' flag to get a more descriptive info.", FLN_VERBOSE);
    return true;
  }

  // TODO: add verbose info...

  return true;
}

static bool run_version(cli_t *restrict cli) {
  // If any other arg than verbose:
  if (FLAG_DIFFERENCE(cli->flags, FCI_VERBOSE) || cli->path_count) {
    FLAG_CAN_ONLY_BE_USED_WITH_VERBOSE_ERR(FLN_VERSION);
    return false;
  }

  // If not verbose:
  else if (!FLAG_INTERSECTION(cli->flags, FCI_VERBOSE))
    printf("%s %s (%s %s)\n",
           cli->self_path, PROGRAM_TAG, PROGRAM_SHORT_COMMIT_HASH, PROGRAM_COMMIT_DATE);

  // Else (verbose):
  else {
    printf("binary: %s\n", cli->self_path);
    printf("commit hash: %s\n", PROGRAM_FULL_COMMIT_HASH);
    printf("commit date: %s\n", PROGRAM_COMMIT_DATE);
    printf("host machine: %s\n", PROGRAM_HOST_MACHINE);
    printf("release tag: %s\n", PROGRAM_TAG);
    printf("compiled with: %s %s\n", PROGRAM_COMPILER, COMPILER_VERSION);
  }

  return true;
}

