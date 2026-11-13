#include "cli.h"
#include "cli_flags.h"
#include "print_defs.h"

#include <stdio.h>
#include <string.h>

#define LIST_AVAILABLE_FLAGS_TIP \
  printerr("Consider using the --%s/-%c flag to list available flags!\n", FLN_HELP, FSN_HELP)

// Temporary flag instance pointer to avoid array index accessing.
static const flag_t *flag_instance;

bool parse_long_flag(cli_t *cli, char *flag_string) {

  // when flag is null/zero length.
  if (!flag_string || !strlen(flag_string)) {
    tagged_err("passing empty flag ('--') isn't allowed at this point.\n");
    LIST_AVAILABLE_FLAGS_TIP;
    return false;
  }

  // for each available global flag.
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {

    flag_instance = &GLOBAL_FLAGS[i];

    // if long name matches:
    if (strcmp(flag_instance->long_name, flag_string) == 0) {
      cli->global_flags |= flag_instance->code_identifier;
      return true;
    }
  }

  // else, flag not found!
  tagged_err("'--%s' is an unknown flag.\n", flag_string);
  LIST_AVAILABLE_FLAGS_TIP;

  return false;
}

bool parse_short_flag(opaque_cli_t *cli, char *short_seq) {
  static char flag_alias;
  static bool treated_alias;

  // When flag is null/zero length.
  if (!short_seq || !strlen(short_seq)) {
    tagged_err("passing empty switch/alias ('-') isn't allowed at this point.\n");
    LIST_AVAILABLE_FLAGS_TIP;
    return false;
  }

  // indexing (for each) chars on short sequence.
  for (size_t si = 0; si < strlen(short_seq); si++) {

    flag_alias = short_seq[si];
    treated_alias = false;

    // indexing (for each) flags on global flags array.
    for (size_t fi = 0; fi < GLOBAL_FLAGS_COUNT; fi++) {

      flag_instance = &GLOBAL_FLAGS[fi];

      // if current switch matches with the current instance.
      if (flag_instance->short_name == flag_alias) {
        treated_alias = true;
        cli->global_flags |= flag_instance->code_identifier;
        break;
      }
    }

    // if current alias not treated.
    if (!treated_alias) {
      tagged_err("'-%c' isn't recognized as valid switch/alias.\n", flag_alias);
      LIST_AVAILABLE_FLAGS_TIP;
      return false;
    }
  }

  return true;
}
