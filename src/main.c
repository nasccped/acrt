#include "cli.h"
#include <stdio.h>

/* Prints directly to stderr. */
#define printerr(ARGS...) fprintf(stderr, ARGS)

/* Prints directly to stderr with an error tag. */
#define tag_printerr(ARGS...) printerr("\x1b[91merror\x1b[0m: " ARGS)

/* Print the error info returned by parsing function. */
static inline void print_parse_error(cli_parse_err_t *err);

/* Print the error info returned by run function. */
static inline void print_run_error(cli_run_err_t *err);

int main(int argc, const char *argv[]) {
  static cli_t cli = { 0 };
  static cli_parse_err_t p_err;
  static cli_run_err_t r_err;

  if ((p_err = cli_parse(&cli, argc, argv)).is_err) {
    print_parse_error(&p_err);
    return 1;
  }

  else if ((r_err = cli_run(&cli)).is_err) {
    print_run_error(&r_err);
    return 1;
  }

  return 0;
}

static inline void print_parse_error(cli_parse_err_t *err) {
  // prints the error based on the error kind.
  switch (err->err_kind) {

    // when arg counting exceeds the max.
    case PARSE_ERROR_EXCEEDS_MAX_PARAMS:
      tag_printerr("this program can handle up to %d params (%d were passed)\n",
                   CLI_MAX_PARAMS, err->arg_count);
      break;

    // when provided arg contains invalid prefix.
    case PARSE_ERROR_PARAM_INVALID_PREFIX:
      tag_printerr("passing argument with invalid prefix (%s)\n", err->param);
      break;

    // When provided param is empty string/flag/switch
    case PARSE_ERROR_EMPTY_PARAM:
      tag_printerr("empty string/flag/switch isn't allowed (%s)\n", err->param);
      break;

    // When the flag wasn't valid at that context.
    case PARSE_ERROR_INVALID_FLAG:
      tag_printerr("provided flag (%s) isn't valid on such context\n", err->flag);
      break;

    // When the switch wasn't valid at that context.
    case PARSE_ERROR_INVALID_SWITCH:
      tag_printerr("provided switch (%c) isn't valid on that context\n", err->flag_switch);
      break;
  }
}

static inline void print_run_error(cli_run_err_t *err) {
  /* print the error based on error kind. */
  switch (err->err_kind) {
    /* provided flag conflicts with other params. */
    case RUN_ERROR_FLAG_CONFLICT:
      tag_printerr("passing %s flag accompanied by %s isn't allowed\n",
                   err->flag_conflict.flag_being_used, err->flag_conflict.cant_be_used_with);
      break;

    /* When test case argument isn't provided. */
    case RUN_ERROR_NO_CASES_WERE_PASSED:
      tag_printerr("no cases were passed to the runtime\n");
      printerr(
        "Consider passing at least one shared object test case as argument. You\n"
        "can also use the --help flag to get usage tips.\n"
      );
      break;
  }
}
