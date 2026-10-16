#include "cli.h"
#include "help_subcommand.h"
#include "version_subcommand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Detect flag usage by flag code identifier. */
#define FLAG_CODE_IS_VERSION(CODE) (CODE) & GLOBAL_FLAG_CODE_VERSION
#define FLAG_CODE_IS_VERBOSE(CODE) (CODE) & GLOBAL_FLAG_CODE_VERBOSE
#define FLAG_CODE_IS_HELP(CODE)    (CODE) & GLOBAL_FLAG_CODE_HELP

/* Remove some flag code from the initial value. */
#define FLAG_CODE_REMOVE(INITIAL, REM_CODE) (INITIAL) & ~(REM_CODE)

/* Refers to the mode of the current argument (long flag, short flag or not a flag (path)). */
typedef enum {
  ARG_MODE_NOT_A_FLAG,
  ARG_MODE_LONG_FLAG,
  ARG_MODE_SHORT_FLAG
} arg_mode_t;

/* Insert a new test case into the cli buffer. This operation can fail if buffer overflow. */
static void add_test_case(cli_t *cli, const char *test_case_path);

/* Updates the flag code pointer based on the provided flag string. This function actually searches
 * for the matching flag on the GLOBAL_FLAGS array and writes on err pointer if some fail occur. */
static void handle_long_flag(flag_code_t *flag_ptr, const char *flag_str, cli_parse_err_t *err);

/* Updates the flag code pointer FOR EACH switch on the entire flag sequence until it reaches the
 * end or it finds some invalid switch. Since the operation can fails, it also requires a pointer
 * to an parsing error. */
static void handle_short_flag(flag_code_t *flag_ptr, const char *flag_seq, cli_parse_err_t *err);

/* Set the arg mode inner value from the argument string. It also requires parse error pointer to
 * check potential invalid arguments. It returns an integer meaning how many chars should be
 * ignored at argument beginning (when dealing with flags). */
static size_t set_arg_mode_from_argument(arg_mode_t *am, const char *argument,
                                         cli_parse_err_t *err);

cli_parse_err_t cli_parse(cli_t *cli, int argc, const char *argv[]) {
  static cli_parse_err_t err = { .is_err = false };
  static arg_mode_t arg_mode;
  static size_t arg_offset;

  /* max args exceeded. */
  if (argc > CLI_MAX_PARAMS) {
    err.is_err = true;
    err.err_kind = PARSE_ERROR_EXCEEDS_MAX_PARAMS;
    err.arg_count = argc;
    return err;
  }

  cli->bin_path = argv[0];

  /* while parsing arg. */
  for (int i = 1; i < argc; i++) {
    arg_offset = set_arg_mode_from_argument(&arg_mode, argv[i], &err);

    /* check if setting generates an error. */
    if (err.is_err)
      return err;

    /* arg offset = argument length means empty param. */
    else if (arg_offset == strlen(argv[i])) {
      err.is_err = true;
      err.err_kind = PARSE_ERROR_EMPTY_PARAM;
      err.param = argv[i];
      return err;
    }

    /* if argument refers to short flag. */
    else if (arg_mode == ARG_MODE_SHORT_FLAG)
      handle_short_flag(&cli->global_flags, argv[i] + arg_offset, &err);

    /* if argument refers to long flag. */
    else if (arg_mode == ARG_MODE_LONG_FLAG)
      handle_long_flag(&cli->global_flags, argv[i] + arg_offset, &err);

    /* otherwise (is a path). */
    else
      add_test_case(cli, argv[i] + arg_offset);
  }

  return err;
}

cli_run_err_t cli_run(cli_t *cli) {
  static cli_run_err_t run_err = { .is_err = false };
  static flag_code_t flag_code;

  flag_code = cli->global_flags;

  /* If flag refers to --help */
  if (FLAG_CODE_IS_HELP(flag_code)) {
    /* When passing any other flag different than verbose. */
    if (FLAG_CODE_REMOVE(flag_code, GLOBAL_FLAG_CODE_HELP | GLOBAL_FLAG_CODE_VERBOSE)) {
      run_err.is_err = true;
      run_err.err_kind = RUN_ERROR_FLAG_CONFLICT;
      run_err.flag_conflict.flag_being_used = FLAG_STRING_HELP;
      run_err.flag_conflict.cant_be_used_with = "any flag other than " FLAG_STRING_VERBOSE;
      return run_err;
    }

    /* When test case paths are provided. */
    else if (cli->test_case_count) {
      run_err.is_err = true;
      run_err.err_kind = RUN_ERROR_FLAG_CONFLICT;
      run_err.flag_conflict.flag_being_used = FLAG_STRING_HELP;
      run_err.flag_conflict.cant_be_used_with = "test case path(s)";
      return run_err;
    }

    /* else, just run help */
    run_help_subcommand(cli->bin_path, FLAG_CODE_IS_VERBOSE(flag_code));
    return run_err;
  }

  /* when calling --version flag. */
  else if (FLAG_CODE_IS_VERSION(flag_code)) {
    /* If any other flag than verbose. */
    if (FLAG_CODE_REMOVE(flag_code, GLOBAL_FLAG_CODE_VERSION | GLOBAL_FLAG_CODE_VERBOSE)) {
      run_err.is_err = true;
      run_err.err_kind = RUN_ERROR_FLAG_CONFLICT;
      run_err.flag_conflict.flag_being_used = FLAG_STRING_VERSION;
      run_err.flag_conflict.cant_be_used_with = "any flag other than " FLAG_STRING_VERBOSE;
      return run_err;
    }

    /* When passing cases path with --version. */
    else if (cli->test_case_count) {
      run_err.is_err = true;
      run_err.err_kind = RUN_ERROR_FLAG_CONFLICT;
      run_err.flag_conflict.flag_being_used = FLAG_STRING_VERSION;
      run_err.flag_conflict.cant_be_used_with = "test case path(s)";
      return run_err;
    }

    /* else, just run version. */
    run_version_subcommand(FLAG_CODE_IS_VERBOSE(flag_code));
    return run_err;
  }

  /* When no implicit subcommand (version or help) + no test case passed. */
  else if (!cli->test_case_count) {
    run_err.is_err = true;
    run_err.err_kind = RUN_ERROR_NO_CASES_WERE_PASSED;
    return run_err;
  }

  /* otherwise (some test case). */
  else {

    printf("Running %d test cases:\n", cli->test_case_count);

    for (int i = 0; i < cli->test_case_count; i++) {
      printf(" [%2d] %s\n", i + 1, cli->test_case_paths[i]);
    }
  }

  return run_err;
}

static void add_test_case(cli_t *cli, const char *test_case_path) {
  /* NOTE: if, for some goddam reason, inner buffer overflows, just panic. This won't occur since
   * argc checking is done at parse function. This is just to ensure visible panicking... */
  if (cli->test_case_count >= CLI_MAX_PARAMS) {
    fprintf(stderr, "fatal[%d]: test case path overflow (should be handle by program logic)\n",
            __LINE__);
    exit(1);
  }

  cli->test_case_paths[cli->test_case_count++] = test_case_path;
}

static size_t set_arg_mode_from_argument(arg_mode_t *am, const char *argument, cli_parse_err_t *err) {
  static size_t arg_offset;

  arg_offset = 0;
  *am = ARG_MODE_NOT_A_FLAG;

  /* for each char within the argument */
  for (size_t i = 0; i < strlen(argument); i++) {

    /* skip blank chars. */
    if (argument[i] == ' ') {
      arg_offset++;
      continue;
    }
  
    /* break at the first non-dash char. */
    else if (argument[i] != '-')
      return arg_offset;
  
    else if (*am == ARG_MODE_NOT_A_FLAG) {
      *am = ARG_MODE_SHORT_FLAG;
      arg_offset++;
    }
  
    else if (*am == ARG_MODE_SHORT_FLAG) {
      *am = ARG_MODE_LONG_FLAG;
      arg_offset++;
    }
  
    /* already long flag. */
    else {
      err->is_err = true;
      err->err_kind = PARSE_ERROR_PARAM_INVALID_PREFIX;
      err->param = argument;
      return arg_offset;
    }
  }

  return arg_offset;
}

static void handle_long_flag(flag_code_t *flag_ptr, const char *flag_str, cli_parse_err_t *err) {
  /* for each available flag. */
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {
    /* if not matches. */
    if (strcmp(flag_str, GLOBAL_FLAGS[i].long_name) != 0)
      continue;

    *flag_ptr |= GLOBAL_FLAGS[i].code;
    return;
  }

  /* reaching this point means no matching found. */
  err->is_err = true;
  err->err_kind = PARSE_ERROR_INVALID_FLAG;
  err->flag = flag_str;
}

static void handle_short_flag(flag_code_t *flag_ptr, const char *flag_seq, cli_parse_err_t *err) {
  static bool treated_switch;

  for (size_t i = 0; i < strlen(flag_seq); i++) {
    treated_switch = false;

    for (size_t f = 0; f < GLOBAL_FLAGS_COUNT; f++) {
      if (GLOBAL_FLAGS[f].short_name != flag_seq[i])
        continue;

      treated_switch = true;
      *flag_ptr |= GLOBAL_FLAGS[f].code;
      break;
    }

    /* if all flags checked and still not matches found. */
    if (!treated_switch) {
      err->is_err = true;
      err->err_kind = PARSE_ERROR_INVALID_SWITCH;
      err->flag_switch = flag_seq[i];
      return;
    }
  }
}
