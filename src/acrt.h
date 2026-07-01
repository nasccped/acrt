#ifndef _ACRT_H_
#define _ACRT_H_

/* Which 'context_name' variant to print? */
typedef enum {
  /* Use the file name as name kind (default). */
  CONTEXT_NAME_USE_FILE_NAME,

  /* Use the function name as name kind. */
  CONTEXT_NAME_USE_FUNCTION_NAME,

  /* Use a custom naming specified by the user. */
  CONTEXT_NAME_USE_CUSTOM_NAME
} acrt_name_kind_t;

/* Naming related stuff to print context at std(out/err). */
struct __acrt_context_name {
  /* Name kind for the current context name. */
  acrt_name_kind_t kind;

  /* Context name inner data. */
  struct {
    const char
        /* Using __FILE__ as context name. */
        *file,

        /* Using __FUNC__ as context name. */
        *function,

        /* Using a custom name. */
        *custom;
  } name;
};

/* Action for failed assertions (tagged enum). */
struct __acrt_context_on_fail {
  /* Which action to execute. */
  enum {
    /* Exit the entire program with the provided exit code at 'action.exit_code'
     * (default). */
    ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,

    /* Ignore all remaining assertions. */
    ON_FAIL_IGNORE_REMAINING_ASSERTIONS,

    /* Continue all remaining assertions. */
    ON_FAIL_CONTINUE_REMAINING_ASSERTIONS,

    /* Print assertion counting and exit the program with the exit code. */
    ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE,

    /* Run a callback function pointer + exit the program with the exit code. */
    ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE,
  } kind;

  /* Action inner data. */
  union {
    /* Exit code when 'ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE' is set. */
    int exit_code;

    /* Struct when 'ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE' is
     * set. */
    struct {
      /* The counting pointer. */
      struct __acrt_counting *counting;

      /* The exit code to return. */
      int exit_code;
    } counting_and_exit;

    /* Struct when 'ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE' is set. */
    struct {
      /* Callback function. */
      void (*callback_function)();

      /* Exit code to return. */
      int exit_code;
    } callback_and_exit;
  } action;
};

/* Assertion counting. */
struct __acrt_counting {
  unsigned int
      /* Total of assertions. */
      total,

      /* Total of passed assertions. */
      passed,

      /* Total of failed assertions. */
      failed,

      /* Total of ignored assertions (when 'ON_FAIL_IGNORE_REMAINING_ASSERTIONS'
       * is set). */
      ignored;
};

/* Display mode for acrt context. */
typedef enum {
  /* Display only failed assertions. */
  DISPLAY_MODE_FAILED_ONLY,

  /* Display both failed and passed assertions. */
  DISPLAY_MODE_FAILED_AND_PASSED,

  /* Display all assertions (even when being ignored). */
  DISPLAY_MODE_ALL,

  /* Don't display any assertion. */
  DISPLAY_MODE_QUIET
} acrt_display_mode_t;

/* Acrt private context to handle. */
struct __acrt_context {
  struct __acrt_context_name name;
  struct __acrt_context_on_fail on_fail;
  struct __acrt_counting counting;
  acrt_display_mode_t display_mode;
};

/* Acrt entrypoint. */
typedef struct {
  struct __acrt_context __context;
} acrt_t;

/* Create a new 'acrt_t' struct with the default inner values. This function
 * takes the __FILE__ and __FUNCTION__ strings. */
acrt_t __acrt_default(const char *, const char *);

/* Creates a new acrt struct. The name and counter is set to default. You can
 * still set a custom name by using 'acrt_set_name' function. */
#define acrt_new() __acrt_default(__FILE__, __FUNCTION__)

/* Set inner acrt on fail variant to exit with exit code (second param
 * (integer)). */
void acrt_set_on_fail_exit_program(acrt_t *, int);

/* Ignore remaining assertions if current one fails. */
void acrt_set_on_fail_ignore_remaining_assertions(acrt_t *);

/* Continue remaining assertions if current one fails. */
void acrt_set_on_fail_continue_remaining_assertions(acrt_t *);

/* Display current assertion counting and exits the program with the exit
 * code. */
void acrt_set_on_fail_print_counting_and_exit_program(acrt_t *, int);

/* Run function callback and exit the program with the exit code. */
void acrt_set_on_fail_run_callback_and_exit_program(acrt_t *, void (*)(), int);

/* Set a new display mode for the acrt pointer. */
void acrt_set_display_mode(acrt_t *, acrt_display_mode_t);

/* Set the acrt pointer context name to file (use same name as the file). */
void acrt_set_context_name_to_file(acrt_t *);

/* Set the acrt pointer context name to function (use same name as the function
 * scope). */
void acrt_set_context_name_to_function(acrt_t *);

/* Set the acrt pointer context name to a custom char pointer. */
void acrt_set_context_name_to_custom(acrt_t *, const char *);

#endif
