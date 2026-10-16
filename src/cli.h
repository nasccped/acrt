#ifndef _ACRT_CLI_H_
#define _ACRT_CLI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Maximum amount of parameters which acrt cli can handle. */
#define CLI_MAX_PARAMS 64

/* Flag string identifiers. */
#define FLAG_STRING_HELP    "help"
#define FLAG_STRING_VERBOSE "verbose"
#define FLAG_STRING_VERSION "version"

/* Use type alias so flag code can be changed whenever necessary. */
typedef uint16_t flag_code_t;

/* When argc/argv parsing to cli struct fails. It stores the fail case like Rust's 'Err' enum
 * variant. */
typedef struct {

  /* Since err_kind default variant also means an error, some error trigger is required to check
   * inner values. */
  bool is_err;

  /* Kind of error that can be generated. */
  enum {

    /* Number of arguments exceeds the maximum that the cli can safely handle. */
    PARSE_ERROR_EXCEEDS_MAX_PARAMS,

    /* Provided argument provides non-valid prefix (such as '---'). The argument value (string) is
     * also stored at 'param' field. */
    PARSE_ERROR_PARAM_INVALID_PREFIX,

    /* When the passed parameter is empty string, flag or switch. */
    PARSE_ERROR_EMPTY_PARAM,

    /* When the provided flag is invalid. */
    PARSE_ERROR_INVALID_FLAG,

    /* When the provided switch (flag alias) is invalid. */
    PARSE_ERROR_INVALID_SWITCH

  } err_kind;

  /* Aux values that follows the error kind. */
  union {
    int arg_count;
    const char *param;
    const char *flag;
    char flag_switch;
  };

} cli_parse_err_t;

/* Cli members definition. Contains required fields to the app run. */
typedef struct {

  /* Command name used to invoke the process. */
  const char *bin_path;

  /* Final code used by flags invocation. */
  flag_code_t global_flags;

  /* NOTE: mentioned test case refers to a single .so file, not a test case scope (function) since
   * they'll handled dynamically at runtime. */

  /* All test cases passed through stdin. */
  const char *test_case_paths[CLI_MAX_PARAMS];

  /* Test case counting. */
  int test_case_count;

} cli_t;

/* Definition of a flag data-type. */
typedef struct {

  /* Shows flag info, such as long name, alias and description text. */
  const char *long_name, short_name, *description;

  /* Stores the flag binary code (used as unique identifier at runtime). */
  flag_code_t code;

} flag_t;

/* Kind of errors that can be occur at runtime (after parse). */
typedef struct {

  bool is_err;

  /* Kind of error. */
  enum {

    /* Some flag invocation conflicts with other args. */
    RUN_ERROR_FLAG_CONFLICT,

    /* When no test cases are passed as argument. */
    RUN_ERROR_NO_CASES_WERE_PASSED

  } err_kind;

  union {

    /* Fields related to the flag conflict kind of error. */
    struct {
      const char *flag_being_used, *cant_be_used_with;
    } flag_conflict;

  };

} cli_run_err_t;

#define _GLOBAL_FLAG_CODE_INIT   0b1
#define GLOBAL_FLAG_CODE_VERSION (_GLOBAL_FLAG_CODE_INIT << 0)
#define GLOBAL_FLAG_CODE_HELP    (_GLOBAL_FLAG_CODE_INIT << 1)
#define GLOBAL_FLAG_CODE_VERBOSE (_GLOBAL_FLAG_CODE_INIT << 2)

/* Collection of flags provided by the cli. */
static const flag_t GLOBAL_FLAGS[] = {
  /* long name             short name   description                    code                    */
  { FLAG_STRING_VERSION,   'V',         "Shows version info",          GLOBAL_FLAG_CODE_VERSION },
  { FLAG_STRING_VERBOSE,   'v',         "Shows the help panel",        GLOBAL_FLAG_CODE_VERBOSE },
  { FLAG_STRING_HELP,      'h',         "Runs the action verbosely",   GLOBAL_FLAG_CODE_HELP    }
};

/* Counting of flags. */
static const size_t GLOBAL_FLAGS_COUNT = sizeof(GLOBAL_FLAGS) / sizeof(flag_t);

/* Does the argc/argv parsing into the cli pointer. Returns a cli parse error type containing the
 * error (if some). */
cli_parse_err_t cli_parse(cli_t *cli, int argc, const char *argv[]);

/* Does the cli running and returns the run error (if some). */
cli_run_err_t cli_run(cli_t *cli);

#endif
