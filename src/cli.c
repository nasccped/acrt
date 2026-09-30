#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Wrap int results into a meaningful identifier (avoid numerical misuse). */
#define CLI_FAILURE 0
#define CLI_SUCCESS 1

/* Flag code identifiers. */
#define _FLAG_CODE_INIT   0b1
#define FLAG_CODE_HELP    _FLAG_CODE_INIT << 0
#define FLAG_CODE_VERSION _FLAG_CODE_INIT << 1
#define FLAG_CODE_DRY_RUN _FLAG_CODE_INIT << 2

/* Help tip placeholder. */
#define HELP_FLAG_TIP(FILE) fprintf((FILE), "Consider using --help flag.\n")

typedef uint8_t arg_mode_t;
typedef struct cli cli_t;
typedef uint8_t flag_code_t;

/* Possible variants when trying to parse an argument. */
typedef enum {
  ARG_MODE_EMPTY_STRING,
  ARG_MODE_SHORT_FLAG,
  ARG_MODE_LONG_FLAG,
  ARG_MODE_NOT_A_FLAG,
  ARG_MODE_MALFORMED
} argument_mode_t;

/* Flag definitions type (helps when parsing). */
typedef struct {
  char *long_name, short_name, *description;
  flag_code_t code;
} flag_definition_t;

/* Flag definitions data. */
static const flag_definition_t FLAG_DEFINITIONS[] = {
  /* flag long name     flag short name     flag description                                           flag code        */
  { "help",             'h',                "Displays the help panel",                                 FLAG_CODE_HELP    },
  { "version",          'v',                "Display the program version",                             FLAG_CODE_VERSION },
  { "dry-run",          'n',                "Simulated execution without actually running test cases", FLAG_CODE_DRY_RUN }
};

/* Keep track of how many flags were disposed. */
static const size_t FLAG_COUNT = sizeof(FLAG_DEFINITIONS) / sizeof(flag_definition_t);

/* Helper function for long flags parsing. It expects the flag's long name identifier
 * (dash-exclusive) and returns an integer where:
 * - '0' means parse fail;
 * - '<OTHER>' means parse success. */
static int long_flag_parse(cli_t *cli, char *s);

/* Helper function for short flag switch parsing. It expects the switches sequence (char array
 * dash-exclusive) and returns an integer where:
 * - '0' means at least ONE switch treating fail;
 * - '<OTHER>' means ALL switches successfully treated. */
static int short_flag_parse(cli_t *cli, char *s);

int cli_parse(cli_t *cli, int argc, char *argv[]) {
  static char *arg, cur_char;
  static int arg_offset;
  static argument_mode_t arg_mode;

  // ignore first arg (refers to binary path).
  argc--;
  argv++;

  // if no args
  if (!argc) {
    fputs("Error: acrt requires at least one flag/path.\n", stderr);
    HELP_FLAG_TIP(stderr);
    return CLI_FAILURE;
  }

  // if exceeds max args
  else if (argc > CLI_MAX_ARGS) {
    fprintf(stderr, "Error: acrt can handle up to %d args (%d were passed).\n",
            CLI_MAX_ARGS, argc);
    return CLI_FAILURE;
  }

  for (int i = 0; i < argc; i++) {
    arg_offset = 0;
    arg = argv[i];
    arg_mode = ARG_MODE_EMPTY_STRING;

    // for each char (or until break)
    for (size_t j = 0; j < strlen(arg); j++) {
      cur_char = arg[j];

      if (cur_char == ' ') {
        arg_offset++;
        continue;
      }

      else if (cur_char == '-') {
        // if '-' is the first non-blank char
        if (arg_mode == ARG_MODE_EMPTY_STRING)
          arg_mode = ARG_MODE_SHORT_FLAG;

        // if is '-' twice
        else if (arg_mode == ARG_MODE_SHORT_FLAG)
          arg_mode = ARG_MODE_LONG_FLAG;

        // means third or more
        else {
          arg_mode = ARG_MODE_MALFORMED;
          break;
        }

        arg_offset++;
        continue;
      }

      // else, it isn't a flag.
      else if (arg_mode == ARG_MODE_EMPTY_STRING)
        arg_mode = ARG_MODE_NOT_A_FLAG;

      // we can break at the end of scope since all if blocks have 'continue'.
      break;
    }

    // behave based on arg_mode.
    switch (arg_mode) {

      case ARG_MODE_EMPTY_STRING:
        fprintf(stderr, "Error: passing empty (%d° arg) strings isn't allowed!\n", i + 1);
        HELP_FLAG_TIP(stderr);
        return CLI_FAILURE;

      case ARG_MODE_MALFORMED:
        fprintf(stderr, "Error: malformed argument '%s'!\n", arg);
        HELP_FLAG_TIP(stderr);
        return CLI_FAILURE;

      case ARG_MODE_SHORT_FLAG:
        if (!short_flag_parse(cli, arg + arg_offset))
          return CLI_FAILURE;
        break;

      case ARG_MODE_LONG_FLAG:
        if (!long_flag_parse(cli, arg + arg_offset))
          return CLI_FAILURE;
        break;

      case ARG_MODE_NOT_A_FLAG:
        cli->paths[cli->path_count++] = arg + arg_offset;
        break;
    }
  }

  return CLI_SUCCESS;
}


int cli_run(struct cli *cli) {
  // TODO: this code is just for preview purpose and must be updated.
  static const flag_definition_t *fd;

  printf("Called flags:\n");

  for (size_t i = 0; i < FLAG_COUNT; i++) {

    fd = &FLAG_DEFINITIONS[i];
    printf("  --%s", fd->long_name);

    if (fd->short_name != 0) {
      printf(" (-%c)", fd->short_name);
    }

    printf(": %s\n", (fd->code & cli->flags) ? "yes" : "no");
  }

  printf("Called paths:\n");

  for (uint8_t i = 0; i < cli->path_count; i++) {
    printf("  %s\n", cli->paths[i]);
  }

  return CLI_SUCCESS;
}

static int long_flag_parse(cli_t *cli, char *s) {
  static const flag_definition_t *fd;

  for (size_t i = 0; i < FLAG_COUNT; i++) {
    fd = &FLAG_DEFINITIONS[i];

    if (strcmp(fd->long_name, s) == 0) {
      cli->flags |= fd->code;
      return CLI_SUCCESS;
    }
  }

  fprintf(stderr, "Error: the provided argument doesn't match with any know flag (--%s)!\n", s);
  HELP_FLAG_TIP(stderr);
  return CLI_FAILURE;
}

static int short_flag_parse(cli_t *cli, char *s) {
  static const flag_definition_t *fd;
  static char cur_char;
  static int treated_char;

  for (size_t s_ind = 0; s_ind < strlen(s); s_ind++) {
    cur_char = s[s_ind];
    treated_char = 0;

    for (size_t f_ind = 0; f_ind < FLAG_COUNT; f_ind++) {
      fd = &FLAG_DEFINITIONS[f_ind];

      if (fd->short_name == cur_char) {
        cli->flags |= fd->code;
        treated_char++;
        break;
      }
    }

    if (!treated_char) {
      fprintf(stderr, "Error: '-%c' switch doesn't match with any know flag!\n", cur_char);
      HELP_FLAG_TIP(stderr);
      return CLI_FAILURE;
    }
  }

  return CLI_SUCCESS;
}
