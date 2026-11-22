#include "cli.h"

#include <string.h>

#define FAE 0   /* flag alias empty (FAE). */
#define FCI 0b1 /* flag code init   (FCI) */

typedef struct cli cli_t;
typedef struct flag flag_t;

// NOTE: all values defined as non-static are accessed by other sources with 'extern' accessor.

const char FLAG_PREFIX = '-';

// Flag definitions:                    name                   alias   description                                             code
const flag_t FLAG_RERUN             = { "rerun",               FAE,    "Allow an input file to be ran more than once",         (FCI << 0) };
const flag_t FLAG_SUPPRESS_WARNINGS = { "suppress-warnings",   'S',    "Suppress all generated warnings",                      (FCI << 1) };
const flag_t FLAG_MAY_EMPTY         = { "may-empty",           FAE,    "Report and exit normally even if no files provided",   (FCI << 2) };
const flag_t FLAG_HELP              = { "help",                'h',    "Prints the help info",                                 (FCI << 3) };
const flag_t FLAG_VERBOSE           = { "verbose",             'v',    "Does the action verbosely",                            (FCI << 4) };
const flag_t FLAG_VERSION           = { "version",             'V',    "Prints the program version info",                      (FCI << 5) };
const flag_t FLAG_DUMP_VERSION      = { "dump-version",        FAE,    "Prints only the version tag annotation",               (FCI << 6) };

const flag_t *GLOBAL_FLAGS[] = {
  &FLAG_RERUN,
  &FLAG_MAY_EMPTY,
  &FLAG_SUPPRESS_WARNINGS,
  &FLAG_HELP,
  &FLAG_VERBOSE,
  &FLAG_VERSION,
  &FLAG_DUMP_VERSION,
};

size_t GLOBAL_FLAGS_COUNT = sizeof(GLOBAL_FLAGS) / sizeof(flag_t *);

cli_t global_cli;

bool flag_alias_matches(const flag_t *restrict flag, char alias) {
  return (flag->alias != FAE) && (flag->alias == alias);
}

bool flag_code_matches(const flag_t *restrict flag, size_t code) {
  return (flag->code & code);
}

size_t flag_code_remove(size_t c1, size_t c2) {
  return (c1 & ~(c2));
}

bool flag_long_name_matches(const flag_t *restrict flag, const char *long_name) {
  return strcmp(flag->name, long_name) == 0;
}
