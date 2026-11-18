#ifndef _ACRT_RUNTIME_HELPERS_H_
#define _ACRT_RUNTIME_HELPERS_H_

#include <stdlib.h>

/* This file provides runtime definitions and macro helpers to avoid out-of-context including
 * between non related source files (turns debugging and implementing little difficulty). */

// acrt's arrays max size (avoid malloc and arena usage).
#define ACRT_BUFFER_MAX_SIZE 64

// Flag code identifiers (FCI) definitions:
#define __FCI_INIT            0b1
#define FCI_DUMP_VERSION      (__FCI_INIT << 0)
#define FCI_HELP              (__FCI_INIT << 1)
#define FCI_MAY_EMPTY         (__FCI_INIT << 2)
#define FCI_SUPPRESS_WARNINGS (__FCI_INIT << 3)
#define FCI_VERBOSE           (__FCI_INIT << 4)
#define FCI_VERSION           (__FCI_INIT << 5)

// Returns the intersection between two flags (A and B).
#define FLAG_INTERSECTION(A, B) ((A) & (B))

// Returns the difference between two flags (A and B).
#define FLAG_DIFFERENCE(A, B) ((A) & ~(B))

// Flag long name (FLN) definitions:
#define FLN_DUMP_VERSION      "dump-version"
#define FLN_HELP              "help"
#define FLN_MAY_EMPTY         "may-empty"
#define FLN_SUPPRESS_WARNINGS "suppress-warnings"
#define FLN_VERBOSE           "verbose"
#define FLN_VERSION           "version"

// Flag short name (FSN) definitions:
#define FSN_DUMP_VERSION       0
#define FSN_HELP              'h'
#define FSN_MAY_EMPTY          0
#define FSN_SUPPRESS_WARNINGS 'S'
#define FSN_VERBOSE           'v'
#define FSN_VERSION           'V'

// Shorthand for fprintf(stderr, ...). Doesn't add newline at the end.
#define PRINTERR(ARGS...) fprintf(stderr, ARGS)

// Prints a tagged error message to stderr + adds a newline at the end.
#define TAGERR(ARGS...) do {               \
  PRINTERR("\x1b[91merror\x1b[0m: " ARGS); \
  PRINTERR("\n");                          \
} while (0)

// Works similar to TAGERR but it takes a FILE pointer to prints to and uses the 'tip' prefix.
#define TAGTIP(FILE_TARG, ARGS...) do {              \
  fprintf((FILE_TARG), "\x1b[96mtip\x1b[0m: " ARGS); \
  fprintf((FILE_TARG), "\n");                        \
} while (0)

// Works similar to TAGTIP but uses the 'warn' prefix.
#define TAGWARN(FILE_TARG, ARGS...) do {              \
  fprintf((FILE_TARG), "\x1b[93mwarn\x1b[0m: " ARGS); \
  fprintf((FILE_TARG), "\n");                         \
} while (0)

/* Data structure referring to a cli flag instance. It must be defined at helpers.h because it's
 * used by both parser.c and runner.c. */
typedef struct {

  // Text related fields (long name, alias and description).
  const char *long_name, alias, *description;

  // Integer code identifier.
  size_t code;

} flag_t;

// Available global flags for the cli parsing.
static const flag_t GLOBAL_FLAGS[] = {
  // long name                 alias                      description                                               code
  { FLN_SUPPRESS_WARNINGS,     FSN_SUPPRESS_WARNINGS,     "Suppress all generated warnings",                        FCI_SUPPRESS_WARNINGS },
  { FLN_MAY_EMPTY,             FSN_MAY_EMPTY,             "Report and exit normally even if no files provided",     FCI_MAY_EMPTY         },
  { FLN_HELP,                  FSN_HELP,                  "Prints the help info",                                   FCI_HELP              },
  { FLN_VERBOSE,               FSN_VERBOSE,               "Does the action verbosely",                              FCI_VERBOSE           },
  { FLN_VERSION,               FSN_VERSION,               "Prints the program version info",                        FCI_VERSION           },
  { FLN_DUMP_VERSION,          FSN_DUMP_VERSION,          "Prints only the version tag annotation",                 FCI_DUMP_VERSION      },
};

// Global flags when searching for a match (iteration method).
static const size_t GLOBAL_FLAGS_COUNT = sizeof(GLOBAL_FLAGS) / sizeof(flag_t);

#endif
