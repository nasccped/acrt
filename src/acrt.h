#ifndef _ACRT_H_
#define _ACRT_H_

// Which 'context_name' variant to print?
typedef enum {

  // Use the file name as name kind (default).
  CONTEXT_NAME_USE_FILE_NAME,

  // Use the function name as name kind.
  CONTEXT_NAME_USE_FUNCTION_NAME,

  // Use a custom naming specified by the user.
  CONTEXT_NAME_USE_CUSTOM_NAME

} acrt_name_kind_t;

// Naming related stuff to print context at std(out/err).
struct __acrt_context_name {

  // Name kind for the current context name.
  acrt_name_kind_t kind;

  const char

      // Using __FILE__ as context name.
      *file,

      // Using __FUNC__ as context name.
      *function,

      // Using a custom name.
      *custom;
};

// Which action to execute on failed assertions.
enum __acrt_on_fail_kind {

  // Exit the entire program with the provided exit code at 'action.exit_code'
  // (default).
  ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,

  // Ignore all remaining assertions.
  ON_FAIL_SKIP_FUTURE_ASSERTIONS,

  // Continue all assertions normally.
  ON_FAIL_CONTINUE_ASSERTIONS,

  // Print assertion counting and exit the program with the exit code.
  ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE,

  // Run a callback function pointer + exit the program with the exit code.
  ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE
};

// Action for failed assertions (tagged enum).
struct __acrt_context_on_fail {

  enum __acrt_on_fail_kind kind;

  // Action inner data.
  union {

    // Exit code when 'ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE' is set.
    int exit_code;

    // Struct when 'ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE' is
    // set.
    struct {

      // The counting pointer.
      struct __acrt_counting *counting;

      // The exit code to return.
      int exit_code;

    } counting_and_exit;

    // Struct when 'ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE' is set.
    struct {

      // Callback function.
      void (*callback_function)(void *);

      // Arg used as callback param.
      void *arg;

      // Exit code to return.
      int exit_code;

    } callback_and_exit;

  } action;
};

// Assertion counting.
struct __acrt_counting {

  unsigned int

      // Total of ran assertions.
      total,

      // Total of passed assertions.
      passed,

      // Total of failed assertions.
      failed,

      // Total of ignored assertions (when 'ON_FAIL_SKIP_FUTURE_ASSERTIONS'
      // is set).
      ignored;
};

// Display mode for acrt context.
typedef enum {

  // Display only failed assertions.
  DISPLAY_MODE_FAILED_ONLY,

  // Display both failed and passed assertions.
  DISPLAY_MODE_FAILED_AND_PASSED,

  // Display all assertions (even when being ignored).
  DISPLAY_MODE_ALL,

  // Don't display any assertion.
  DISPLAY_MODE_QUIET

} acrt_display_mode_t;

// Acrt private context to handle.
struct __acrt_context {

  // Naming related context.
  struct __acrt_context_name name;

  // On fail action related context.
  struct __acrt_context_on_fail on_fail;

  // Assertion counting related context.
  struct __acrt_counting counting;

  // Displaying related content.
  acrt_display_mode_t display_mode;

  // If the latest assertion was failed.
  int latest_was_failed;
};

// Acrt entrypoint.
typedef struct {

  // Inner (private) context.
  struct __acrt_context __context;

} acrt_t;

// Create a new 'acrt_t' struct with the default inner values. This function
// takes the __FILE__ and __FUNCTION__ strings.
acrt_t __acrt_default(const char *, const char *);

// Creates a new acrt struct. The naming and counter is set to default. You can
// still set a custom name by using 'acrt_set_name' function.
#define ACRT_NEW() __acrt_default(__FILE__, __FUNCTION__)

// Set a new display mode the self acrt pointer.
void acrt_display_mode(acrt_t *self, acrt_display_mode_t mode);

// Set 'on fail' field to continue future assertions (even if the current one
// fails).
void acrt_on_fail_continue_assertions(acrt_t *self);

// Exits the program with the provided exit code at the first failed assertion.
void acrt_on_fail_exit_program(acrt_t *self, int code);

// Set 'on fail' field to print counting (assertions) and exit program with the
// provided integer code.
void acrt_on_fail_print_counting_and_exit(acrt_t *self, int code);

// Set 'on fail' field to run callback function pointer with provided args +
// exit with the provided integer code.
//
// The callback function expects a void pointer argument (refer to 'arg' param).
//
// If 'self' or 'callback' param is null, the operation fails. The 'arg' param
// can be null since the function can (or not) receive/use the param.
void acrt_on_fail_run_callback_and_exit(acrt_t *self, void (*callback)(void *),
                                        void *arg, int code);

// Set 'on fail' field to skip future assertions if the current one fails.
void acrt_on_fail_skip_assertions(acrt_t *self);

// Reset the self counting pointer to it's initial state (zero items).
void acrt_reset_counting(acrt_t *self);

// Sets a custom name context to the self acrt pointer. The name parameter must
// be a non-null/zero length string, otherwise, operation fails.
void acrt_set_name_to_custom(acrt_t *self, const char *name);

// Sets context name kind to 'CONTEXT_NAME_USE_FILE_NAME'.
void acrt_set_name_to_file(acrt_t *self);

// Sets context name kind to 'CONTEXT_NAME_USE_FUNCTION_NAME'.
void acrt_set_name_to_function(acrt_t *self);

#endif
