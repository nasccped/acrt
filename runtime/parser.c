#include "cli.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>

#define A_FLAG_LISTING_OVERVIEW_STR "a flag listing overview"
#define USE_HELP_TO_GET_TIP(RETRIEVES)                   \
  PRINTERR("\nConsider using '--%s'/'-%c' to get %s!\n", \
           FLAG_HELP.name, FLAG_HELP.alias, (RETRIEVES))

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
enum argument_mode {
  AM_PATH       = 0,
  AM_SHORT_FLAG = 1,
  AM_LONG_FLAG  = 2,
};

typedef bool (*arghand_fn)(const char *);
typedef enum argument_mode argmode_t;
typedef struct cli cli_t;
typedef struct flag flag_t;

static inline arghand_fn arghand_from_argmode(argmode_t);
static bool handle_long_flag(const char *);
static bool handle_path(const char *);
static bool handle_short_flag(const char *);

static const flag_t *flag_ref;
extern cli_t global_cli;
extern const flag_t FLAG_HELP;
extern const char FLAG_PREFIX;
extern const flag_t *GLOBAL_FLAGS[];
extern const size_t GLOBAL_FLAGS_COUNT;

static inline arghand_fn arghand_from_argmode(argmode_t am) {
  return
    (am == AM_PATH)       ? handle_path       :
    (am == AM_SHORT_FLAG) ? handle_short_flag :
    (am == AM_LONG_FLAG)  ? handle_long_flag  :
    NULL; // fallback.
}

bool cli_parse(int argc, const char **argv) {
  static arghand_fn ah;
  static argmode_t am;

  if (argc < 1) {
    TAGERR("'argc' argument from 'cli_parse' function must be greater than 0.");
    PRINTERR("\nThis is such an unexpected behavior!\n");
    return false;
  }

  // NOTE: missing input file(s) no longer checked at parse. It's checked at run!

  for (int ai = 0; ai < argc; ai++) {
    if (ai == 0) {
      global_cli.self_path = argv[ai];
      continue;
    }

    am = AM_PATH;

    for (size_t ci = 0; ci < strlen(argv[ai]); ci++) {
      if (argv[ai][ci] != FLAG_PREFIX)
        break;
      am++;
    }

    if ((size_t)am >= strlen(argv[ai]) || !(ah = arghand_from_argmode(am))) {
      TAGERR("'%s' argument can't be treated as valid path or flag.", argv[ai]);
      return false;
    }

    else if (!ah(argv[ai] + am))
      return false;
  }

  return true;
}

static bool handle_long_flag(const char *flag_name) {
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {
    flag_ref = GLOBAL_FLAGS[i];

    if (flag_long_name_matches(flag_ref, flag_name)) {
      global_cli.flags |= flag_ref->code;
      return true;
    }
  }

  TAGERR("'--%s' is an unknown flag.", flag_name);
  USE_HELP_TO_GET_TIP(A_FLAG_LISTING_OVERVIEW_STR);
  return false;
}

static bool handle_path(const char *path) {
  if (global_cli.path_count >= ACRT_BUFFER_MAX_SIZE) {
    TAGERR("%s can handle up to %d paths.", global_cli.self_path, ACRT_BUFFER_MAX_SIZE);
    return false;
  }

  global_cli.paths[global_cli.path_count++] = path;
  return true;
}

static bool handle_short_flag(const char *switch_seq) {
  static bool treated_char;

  for (size_t i = 0; i < strlen(switch_seq); i++) {
    treated_char = false;

    for (size_t f = 0; f < GLOBAL_FLAGS_COUNT; f++) {
      flag_ref = GLOBAL_FLAGS[f];

      if (flag_alias_matches(flag_ref, switch_seq[i])) {
        treated_char = true;
        global_cli.flags |= flag_ref->code;
        break;
      }
    }

    if (!treated_char) {
      TAGERR("'-%c' is an unknown switch.", switch_seq[i]);
      USE_HELP_TO_GET_TIP(A_FLAG_LISTING_OVERVIEW_STR);
      return false;
    }
  }

  return true;
}
