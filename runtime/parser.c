#include "helpers.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>

// String tip used as 'RETRIEVES' at 'USE_HELP_TO_GET_TIP' macro:
#define A_FLAG_LISTING_OVERVIEW_STR "a flag listing overview"

/* Updates the 'return_value' to fail (false) and go to the 'return_block' on cli_parse function.
 * NOTE: keep this macro as function syntax to express operation meaning! */
#define FAIL_AND_GOTO() do { \
  return_value = false;      \
  goto return_block;         \
} while (0)

// Char identifier used as flag/switch prefix.
#define FLAG_PREFIX '-'

// Stderr tip shorthand when returning error at some handler scope.
#define USE_HELP_TO_GET_TIP(RETRIEVES) do {                                              \
  PRINTERR("Consider using '--%s'/'-%c' to get %s!\n", FLN_HELP, FSN_HELP, (RETRIEVES)); \
} while (0)

// Type alias for argument handler functions (ahf).
typedef bool (*ahf_t)(cli_t *restrict, const char *);

/* Definition of an argument mode. Since acrt runtime can deal with paths and flags only, there's
 * three kind of arguments:
 * - path literal, which mean a file contained in the current directory (or absolute path too);
 * - short flag sequence, which means a sequence (string/char array) of flag switches;
 * - long flag name, which means a single flag long name identifier.
 *
 * Note that each one of it's variants means a different integer value. These integers means the
 * head offset when handling the argument, like:
 * - 'AM_PATH'       => 0 because path literals have no prefix indicator;
 * - 'AM_SHORT_FLAG' => 1 because short flag sequence have 1 char length on prefix indicator ('-')
 * - 'AM_LONG_FLAG'  => 2 because long flag names have 2 char length on prefix indicator ('--').
 *
 * These same offsets are used before passing argument string to a specific handler.
 *
 * NOTE: the latest variant (AM_VARIANT_COUNT) purpose is only to ensure tracking of variant out of
 * bound (since arithmetic operations are done with this type) without using switch statements. */
typedef enum {
  AM_PATH       = 0,
  AM_SHORT_FLAG = 1,
  AM_LONG_FLAG  = 2,

  AM_VARIANT_COUNT
} argument_mode_t;

/* Returns a argument handler function pointer from a given argument mode variant. It can return
 * NULL if unreachable variant. */
static inline ahf_t argument_handler_from_argument_mode(argument_mode_t);

/* Tries to handle the passed long flag name (const char *) on the cli pointer and returns an boolean
 * meaning the operation success. The error printing (if it occurs) is done at function's scope. */
static bool handle_long_flag(cli_t *restrict, const char *);

/* Tries to handle the passed path (const char *) on the cli pointer and returns an boolean meaning
 * the operation success. The error printing (if it occurs) is done at function's scope. */
static bool handle_path(cli_t *restrict, const char *);

/* Tries to handle the passed switch sequence (const char *) on the cli pointer and returns an
 * boolean meaning the operation success. The error printing (if it occurs) is done at function's
 * scope. */
static bool handle_short_flag(cli_t *restrict, const char *);

bool cli_parse(cli_t *restrict cli, int argc, const char **argv) {
  static ahf_t ahf;
  static argument_mode_t am;
  static bool return_value = true;

  // If argc unexpectedly 0 (or less):
  if (argc < 1) {
    TAGERR("'argc' argument from 'cli_parse' function must be greater than 0.");
    PRINTERR("This is such an unexpected behavior!\n");
    FAIL_AND_GOTO();
  }

  /* NOTE: missing input file(s) no longer checked at parse. It's checked at run now!
  else if (argc == 1) {
    report input missing...
  }
  */

  // Iterates on argument index.
  for (int ai = 0; ai < argc; ai++) {

    // If first arg (executable path):
    if (ai == 0) {
      cli->self_path = argv[ai];
      continue;
    }

    am = AM_PATH;

    // Iterates on string index.
    for (size_t si = 0; si < strlen(argv[ai]); si++) {

      // If current char not a flag prefix.
      if (argv[ai][si] != FLAG_PREFIX)
        break;

      // Else, go next argument mode variant.
      am++;
    }

    // if flag prefix ()
    if (am >= AM_VARIANT_COUNT || (size_t)am >= strlen(argv[ai])) {
      TAGERR("'%s' argument can't be treated as valid path or flag.", argv[ai]);
      FAIL_AND_GOTO();
    }

    ahf = argument_handler_from_argument_mode(am);

    if (!ahf(cli, argv[ai] + am))
      FAIL_AND_GOTO();
  }

return_block:
  return return_value;
}

static inline ahf_t argument_handler_from_argument_mode(argument_mode_t am) {
  return (am == AM_PATH)       ? handle_path
       : (am == AM_SHORT_FLAG) ? handle_short_flag
       : (am == AM_LONG_FLAG)  ? handle_long_flag
       : NULL;
}

static bool handle_long_flag(cli_t *restrict cli, const char *flag_name) {
  // For each flag instance:
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {

    // If long name matches:
    if (strcmp(flag_name, GLOBAL_FLAGS[i].long_name) == 0) {
      cli->flags |= GLOBAL_FLAGS[i].code;
      return true;
    }

  }

  // No match found:
  TAGERR("'--%s' is an unknown flag.", flag_name);
  USE_HELP_TO_GET_TIP(A_FLAG_LISTING_OVERVIEW_STR);
  return false;
}

static bool handle_path(cli_t *restrict cli, const char *path) {
  // Error if path count exceeds buffer size.
  if (cli->path_count >= ACRT_BUFFER_MAX_SIZE) {
    TAGERR("%s can handle up to %d paths.", cli->self_path, ACRT_BUFFER_MAX_SIZE);
    return false;
  }

  cli->paths[cli->path_count++] = path;
  return true;
}

static bool handle_short_flag(cli_t *restrict cli, const char *switch_seq) {
  static bool treated_char;

  // For each char within switch sequence:
  for (size_t i = 0; i < strlen(switch_seq); i++) {

    treated_char = false;

    // For each flag instance:
    for (size_t f = 0; f < GLOBAL_FLAGS_COUNT; f++) {

      // If alias matches:
      if (GLOBAL_FLAGS[f].alias == switch_seq[i]) {
        treated_char = true;
        cli->flags |= GLOBAL_FLAGS[f].code;
        break;
      }
    }

    // If current char wasn't treated:
    if (!treated_char) {
      TAGERR("'-%c' is an unknown switch.", switch_seq[i]);
      USE_HELP_TO_GET_TIP(A_FLAG_LISTING_OVERVIEW_STR);
      return false;
    }
  }

  return true;
}
