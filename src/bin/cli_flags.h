#ifndef _ACRT_CLI_FLAGS_H_
#define _ACRT_CLI_FLAGS_H_

#include <stdlib.h>
#include <stdbool.h>

// Flag code identifiers (FCI) definitions.
#define _INITIAL_FCI_VALUE 0b1
#define FCI_HELP          (_INITIAL_FCI_VALUE << 0)
#define FCI_VERSION       (_INITIAL_FCI_VALUE << 1)

// Flag long name (FLN) definitions.
#define FLN_HELP    "help"
#define FLN_VERSION "version"

// Flag short name/alias (FSN) definitions.
#define FSN_HELP    'h'
#define FSN_VERSION 'V'

// 'fci' type referring to 'flag code identifier' which works like an unique unsigned integer
// identifier attached to the self flag instance.
typedef size_t fci_t;

// Use opaque type definition. This allows cli header to be only included at c files instead of
// messing with multi-imports and cyclic including hell.
typedef cli_t opaque_cli_t;

// Data structure referring to the cli flag instance.
typedef struct {

  // View representation of the flag itself, such as names (long and alias) + its description.
  char *long_name, short_name, *string_description;

  // Code value that identifies this flag as unique.
  fci_t code_identifier;

} flag_t;

// Globally available flags by cli parsers.
static const flag_t GLOBAL_FLAGS[] = {
  // long name       short name       description                    code
  { FLN_HELP,        FSN_HELP,        "Prints the help panel",       FCI_HELP    },
  { FLN_VERSION,     FSN_VERSION,     "Prints the version info",     FCI_VERSION },
};

// Counting of global flags (used on loop/linear searches).
static const size_t GLOBAL_FLAGS_COUNT = sizeof(GLOBAL_FLAGS) / sizeof(flag_t);

// Does the flag parsing by taking the 'flag_string' long name and comparing with the available
// global flags. The update is done in place by using the cli pointer. The value returned is a
// boolean, where:
// - 'true' means succeeded parsing;
// - 'false' means failed parsing.
//
// The error (if occurs) is reported at the functions inner scope.
bool parse_long_flag(opaque_cli_t *cli, char *flag_string);

// Does the flag parsing by taking the 'short_seq' sequence of flag alias and then, changes the cli
// pointer inner stated. It works basically the same as 'parse_long_flag' in terms of returning and
// reporting. Note that this function only returns true if ALL the switches/alias refers to well
// known flags.
bool parse_short_flag(opaque_cli_t *cli, char *short_seq);

#endif
