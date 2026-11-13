#include "cli.h"
#include "cli_flags.h"
#include "print_defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Type alias for argument handler function pointer.
typedef bool (*arg_handler_t)(cli_t *, char *);

// Different modes of arguments. The integer value also means the char offset used when parsing the
// argument into the cli fields.
typedef enum {
  AM_PATH       = 0, // Argument is initially a path.
  AM_SHORT_FLAG = 1, // Argument becomes a flag switch/alias sequence (offset by 1 '-').
  AM_LONG_FLAG  = 2  // Argument becomes a flag long name (offset by 2 '--').
} argument_mode_t;

// Adds the path argument to cli inner fields. Can fail if empty string provided. The error
// printing is done at function's scope.
bool add_path(cli_t *cli, char *path);

bool parse_cli(cli_t *cli, int argc, char *argv[]) {
  static argument_mode_t arg_mode;
  static arg_handler_t arg_handler;
  static char *cur_arg;

  cli->self_path = argv[0];

  // when no args.
  if (argc < 2) {
    tagged_err("no args were provided.\n");
    printerr("Consider using --%s/-%c.\n", FLN_HELP, FSN_HELP);
    return false;
  }

  // when argument counting exceeds the max allowed.
  else if ((argc - 1) > BUFFER_MAX_SIZE) {
    tagged_err("%s can handle up to %d args, but %d were provided.\n",
               cli->self_path, BUFFER_MAX_SIZE, argc - 1);
    return false;
  }

  // indexing (for each) on args.
  for (int ai = 1; ai < argc; ai++) {

    // argument mode initially as path.
    arg_mode = AM_PATH;
    cur_arg = argv[ai];

    // indexing on current arg chars.
    for (size_t si = 0; si < strlen(cur_arg); si++) {

      // when not dash (string prefix ended).
      if (cur_arg[si] != '-')
        break;

      // increase arg mode to next variant.
      arg_mode++;
    }

    switch (arg_mode) {
      // mode refers to path.
      case AM_PATH:
        arg_handler = add_path;
        break;

      // mode refers to short name flag.
      case AM_SHORT_FLAG:
        arg_handler = parse_short_flag;
        break;

      // mode refers to long name flag.
      case AM_LONG_FLAG:
        arg_handler = parse_long_flag;
        break;

      // unreachable state (none of above).
      default:
        tagged_err("The '%s' argument can't be handled in a valid state "
                   "(not a path or valid flag).\n", cur_arg);
        return false;
    }

    // if argument handling fails.
    if (!arg_handler(cli, cur_arg + arg_mode))
      return false;
  }

  return true;
}

bool add_path(cli_t *cli, char *path) {

  // if path is null/empty string.
  if (!path || !strlen(path)) {
    tagged_err("empty paths aren't allowed.\n");
    return false;
  }

  // NOTE: indiscriminated incrementing is safe since we already checked the argc value.
  cli->paths[cli->path_count++] = path;
  return true;
}
