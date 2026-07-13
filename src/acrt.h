/* ****************************************************************************
 * MIT License
 *
 * Copyright (c) 2026 nasccped
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ************************************************************************* */

#ifndef _ACRT_H_
#define _ACRT_H_

// It's used since some function signatures requires 'FILE' type.
#include <stdint.h>
#include <stdio.h>

// Assertion counting.
struct __acrt_counting {

  unsigned int

      // Total of ran assertions.
      total,

      // Total of failed assertions.
      failed,

      // Total of ignored assertions (when 'ON_FAIL_SKIP_FUTURE_ASSERTIONS'
      // is set).
      ignored;

  // Total of passed assertions.
  struct {

    unsigned int

        // With warnings.
        with_warnings,

        // With no warnings (clean pass).
        without_warnings;

  } passed;
};

// Acrt entrypoint.
typedef struct {

  // Context name for the self acrt instance.
  struct {

    // Kind of naming being used.
    enum {

      // Use the file name as name kind (default).
      CONTEXT_NAME_USE_FILE_NAME,

      // Use a custom naming specified by the user.
      CONTEXT_NAME_USE_CUSTOM_NAME

    } kind;

    // Data being held. The 'right' char pointer to use is determined by the
    // 'context_name.kind' field.
    struct {

      const char

          // Using __FILE__ as context name.
          *file,

          // Using a custom name.
          *custom;

    } data;

  } context_name;

  // Data hold/action at failed assertions.
  struct {

    // Which data field to execute/use.
    enum {

      // Just exit the running process with an exit code (integer) value.
      ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,

      // Ignore all remaining assertions.
      ON_FAIL_DISABLE_ASSERTIONS,

      // Increment counting.failed value and continue all assertions normally.
      ON_FAIL_CONTINUE_ASSERTIONS,

      // Print assertion counting and exit the program with the exit code.
      ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE,

      // Run a callback function + exit the program with the exit code.
      ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE,

      // Run a callback function but without exiting the current process.
      ON_FAIL_RUN_CALLBACK_AND_DO_NOT_EXIT,

    } action_kind;

    // Data to be used according to 'on_fail.action_kind' variant.
    union {

      // Process exit code when 'ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE' is set.
      int code;

      // Used data whe 'ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE'
      // is set.
      struct {

        // Print data referenced by the counting address.
        struct __acrt_counting *counting;

        // Exit process with the following exit code.
        int code;

      } print_counting_and_exit;

      // Used data when 'ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE' is set.
      struct {

        // Function callback to be ran.
        void (*callback)(void *);

        // Data pointer to be used as callback param.
        void *arg;

        // Exit code value for the current process.
        int code;

      } run_callback_and_exit;

      // Used data when 'ON_FAIL_RUN_CALLBACK_AND_DO_NOT_EXIT' is set.
      struct {

        // Function callback to be ran.
        void (*callback)(void *);

        // Data pointer to be used as callback param.
        void *arg;

        // Do remaining assertions after callback run?
        int continue_assertions;

      } run_callback_and_do_not_exit;

    } data;

  } on_fail;

  // Assertion counting for the self acrt instance.
  struct __acrt_counting counting;

  // If the previous assertion was failed.
  int previous_assertion_failed;

  // Assertion displaying.
  enum {

    // Display only failed assertions.
    DISPLAY_MODE_FAILED_ONLY,

    // Display all assertions.
    DISPLAY_MODE_ALL,

    // Don't display any assertion.
    DISPLAY_MODE_QUIET

  } display_mode;

} acrt_t;

// Private function that runs a boolean assertion from a given number
// (bool-compatible).
//
// It takes an acrt pointer (returns 0 if null), the current source code line,
// and the number to be asserted.
//
// It'll returns 1 if the number is any non-zero value.
int __acrt_run_boolean_assertion_from_number(acrt_t *self,
                                             const unsigned int line,
                                             uintptr_t number);

// Run a boolean assertion over a void pointer value.
//
// It takse an acrt pointer (returns 0 if null), the current source code line,
// and the pointer to be asserted.
//
// It'll returns 1 if pointer holding a non-null address.
int __acrt_run_boolean_assertion_from_pointer(acrt_t *self,
                                              const unsigned int line,
                                              uintptr_t address);

// Creates a new acrt struct. The naming and counter is set to default. You can
// still set a custom name by using 'acrt_set_context_name' function.
#define ACRT_DEFAULT                                                           \
  (acrt_t) {                                                                   \
    .context_name = {.kind = CONTEXT_NAME_USE_FILE_NAME,                       \
                     .data = {.file = __FILE__, .custom = NULL}},              \
    .counting = (struct __acrt_counting){0},                                   \
    .display_mode = DISPLAY_MODE_FAILED_ONLY,                                  \
    .on_fail = {.action_kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE,            \
                .data.code = 1},                                               \
    .previous_assertion_failed = 0,                                            \
  }

// Runs a bool-like assertion.
#define ACRT_BOOL(SELF, VALUE)                                                 \
  __builtin_choose_expr(__builtin_classify_type((VALUE)) == 5,                 \
                        __acrt_run_boolean_assertion_from_pointer(             \
                            (SELF), __LINE__, (uintptr_t)(VALUE)),             \
                        __acrt_run_boolean_assertion_from_number(              \
                            (SELF), __LINE__, (uintptr_t)(VALUE)))

// Display the inner counting data. This function requires an acrt pointer + a
// file target to diplay (stdout/stderr).
//
// Does nothing if any of param is NULL.
void acrt_display_counting(acrt_t *self, FILE *f);

// Set 'on fail' field to continue future assertions (even if the current one
// fails).
void acrt_set_on_fail_continue_assertions(acrt_t *self);

// Set 'on fail' field to skip future assertions if the current one fails.
void acrt_set_on_fail_disable_assertions(acrt_t *self);

// Exits the program with the provided exit code at the first failed assertion.
void acrt_set_on_fail_exit_program(acrt_t *self, int code);

// Set 'on fail' field to print counting (assertions) and exit program with the
// provided integer code.
void acrt_set_on_fail_print_counting_and_exit(acrt_t *self, int code);

// Set 'on fail' field to run callback function with provided args + exit with
// the provided integer code.
//
// The callback function expects a void pointer argument (refer to 'arg' param).
//
// If 'self' or 'callback' param is null, the operation fails. The 'arg' param
// can be null since the function can (or not) receive/use the param.
void acrt_set_on_fail_run_callback_and_exit(acrt_t *self,
                                            void (*callback)(void *), void *arg,
                                            int code);

// Set 'on fail' field to run callback function with the provided arg but
// without exiting the current process.
//
// The callback function expects a void pointer argument (refer to 'arg' param).
//
// If 'self' or 'callback' param is null, the operation fails. The 'arg' param
// can be null since the function can (or not) receive/use the param.
//
// This function also takes a bool (integer) param which decides if remaining
// assertions should continue after callback:
// - '0' means to skip assertions.
// - '<other>' means to continue assertions.
void acrt_set_on_fail_run_callback_without_exit(acrt_t *self,
                                                void (*callback)(void *),
                                                void *arg,
                                                int continue_assertions);

// Reset the assertions counting and previous_assertion_failed fields only.
void acrt_reset_assertions_state(acrt_t *self);

// Set the acrt display mode field to 'DISPLAY_MODE_ALL' variant.
void acrt_set_display_mode_to_all(acrt_t *self);

// Set the acrt display mode field to 'DISPLAY_MODE_FAILED_ONLY' variant.
void acrt_set_display_mode_to_failed_only(acrt_t *self);

// Set the acrt display mode field to 'DISPLAY_MODE_QUIET' variant.
void acrt_set_display_mode_to_quiet(acrt_t *self);

// Sets a custom name context to the self acrt pointer. The name parameter must
// be a non-null/zero length string, otherwise, operation fails.
void acrt_set_context_name_to_custom(acrt_t *self, const char *name);

// Sets context name kind to 'CONTEXT_NAME_USE_FILE_NAME'.
void acrt_set_context_name_to_file(acrt_t *self);

#endif
