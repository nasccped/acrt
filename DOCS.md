# acrt library documentation

This page provides the `acrt` library code description as a whole. It covers the functions, macros
and common types defined within the library source file. You can read it all the way or just use
the following **table of contents**:

- [about the library](#about-the-library)
  - [structs](#structs)
    - [`acrt`](#acrt-struct)
    - [`acrt counting`](#acrt-counting-struct)
    - [`acrt result`](#acrt-result-struct)
  - [macros](#macros)
    - [`ACRT_DEFAULT`](#acrt_default-macro)
    - [`ACRT_BOOL`](#acrt_bool-macro)
  - [functions](#functions)
    - [public functions](#public-functions)
      - [`acrt_display_counting`](#acrt_display_counting-function)
      - [`acrt_reset_assertions_state`](#acrt_reset_assertions_state-function)
      - [`acrt_set_context_name_to_custom`](#acrt_set_context_name_to_custom-function)
      - [`acrt_set_context_name_to_file`](#acrt_set_context_name_to_file-function)
      - [`acrt_set_on_fail_continue_assertions`](#acrt_set_on_fail_continue_assertions-function)
      - [`acrt_set_on_fail_disable_assertions`](#acrt_set_on_fail_disable_assertions-function)
      - [`acrt_set_on_fail_exit_program`](#acrt_set_on_fail_exit_program-function)
      - [`acrt_set_on_fail_print_counting_and_exit`](#acrt_set_on_fail_print_counting_and_exit-function)
      - [`acrt_set_on_fail_run_callback_and_exit`](#acrt_set_on_fail_run_callback_and_exit-function)
      - [`acrt_set_on_fail_run_callback_without_exit`](#acrt_set_on_fail_run_callback_without_exit-function)
      - [`acrt_set_display_mode_to_all`](#acrt_set_display_mode_to_all-function)
      - [`acrt_set_display_mode_to_failed_only`](#acrt_set_display_mode_to_failed_only-function)
      - [`acrt_set_display_mode_to_quiet`](#acrt_set_display_mode_to_quiet-function)
    - [semi-public functions](#semi-public-functions)
      - [`__acrt_run_boolean_assertion`](#__acrt_run_boolean_assertion-function)
    - [private functions](#private-functions)
      - [`display_acrt_result`](#display_acrt_result-function-at-acrt_displayh)
      - [`display_counting_data`](#display_counting_data-function-at-acrt_displayh)
      - [`acrt_result_is_passed`](#acrt_result_is_passed-function-at-acrt_resulth)
      - [`acrt_result_new_bool`](#acrt_result_new_bool-function-at-acrt_resulth)
- [unit tests](#unit-tests): a little about project's testing
  - [test building](#test-building): compiling properly
  - [test running](#test-running): running properly
- [usage](#usage): about `acrt` super type and API exposure
  - [context name](#context-name): `acrt`'s context name field
  - [failure behavior](#failure-behavior): `acrt`'s on fail field
  - [display mode](#display-mode): `acrt`'s display mode field
  - [counting object](#counting-object): `acrt`'s counting field
  - [assertions](#assertions): how to use provided macros
    - [return value](#return-value): value returned from an assertion macro call
  - [context name setters](#context-name-setters): setter functions referring to context name field
  - [on fail setters](#on-fail-setters): setter functions referring to on fail field
  - [display mode setters](#display-mode-setters): setter functions referring to display mode field
  - [reset state setter](#reset-state-setter): reset the `acrt` assertions state to default
  - [counting display](#counting-display): display the counting table

## About the library

The `acrt` library was majorly made with the purpose of flexible assertions and runtime control.
It's disposed as a couple of files but just including the library main header should be enough:
```c
#include "path/to/acrt.h"
```

### Structs

This library currently provides two different structs:
1. [`acrt_t`](#acrt-struct) which is visible and must be used as assertion manager
2. [`acrt counting`](#acrt-counting-struct) which is _dunder private_ and stores the assertion
   counting at runtime

There's also the [`acrt_result`](#acrt-result-struct) which is defined at `acrt_result.h` and used
only by `acrt.c` **local-scope** functions.

#### `acrt` struct

```c
typedef struct { ... } acrt_t;
```

The `acrt` struct is defined by the `acrt_t` naming and it holds a bunch of anonymous `struct` /
`enum` fields which helps the inner state _"unavailability"_.

This value shouldn't be initialized or handled manually. Instead, the caller program should use the
[`functions`](#functions) / [`macros`](#macros) provided by the header.

#### `acrt counting` struct

```c
struct __acrt_counting { ... };
```

The `acrt counting` struct is a common type stored within an `acrt` struct instance and shared with
`acrt` library implementors. It stores the counting of ran assertions + it's statuses as well.

It's updated and checked at runtime by the functions defined at `acrt.c` file _(not visible)_.

#### `acrt result` struct

```c
typedef struct { ... } acrt_result_t;
```

The `acrt result` isn't provided by library but used as a wrapper storing assertion result, kind
and possible warnings. It's defined at `acrt_result.h` header and only included within dependents
implementors.

### Macros

The `acrt` lib provides a set of macros to use along type and function definitions:

#### `ACRT_DEFAULT` macro

```c
#define ACRT_DEFAULT (acrt_t){ ... }
```

The `ACRT_DEFAULT` macro is a placeholder for an `acrt_t` instance with default fields. In previous
versions, the `acrt_t` struct were built through the `__acrt_default( ... )` function call, but
this doesn't allow a global scope initialized object, forcing the user to initialize it within the
`main` or other function scope.

> [!TIP]
>
> The `acrt_t` object inner state can be change by using the setter functions (read
> [functions](#functions) section).

#### `ACRT_BOOL` macro

```c
#define ACRT_BOOL(SELF, VALUE) ...
```

Runs a boolean assertion by taking an `acrt_t` pointer and a expression that can be evaluated to
bool:
```c
acrt_t acrt = ACRT_DEFAULT;
ACRT_BOOL(&acrt, 1);
ACRT_BOOL(&acrt, 3 > 2);
ACRT_BOOL(&acrt, "char pointer");
...
```

This macro call is just a wrapper for a _dunder-private_ function defined at `acrt.h`. It actually
converts the `VALUE` param to a `uintptr_t`, pass it as function param and re-cast to an `int` or
`void *` depending on the type being asserted. That's the only way that I found to accept
literal, pointer and numeric variables without compile warnings.

In short, the assertion will be passed when given `VALUE` refers to non-zero / non `NULL`.

##### Type casting

As mentioned above, to turn the `VALUE` param acceptable, it must be cast to an `uintptr_t` value.
In other words, if it can be casted with `(uintptr_t)(VALUE)` syntax, it's valid!

> [!IMPORTANT]
>
> 1. This cast occurs at macro's expansion, so you don't need to prefix the expression value with
>    casting type.
> 2. The passed value is re-cast to `int` / `void *` depending on the value's type (which is also
>    checked by the macro's expansion). This means that:
>    - `-1` gonna be `-1`
>    - `4.99` gonna be `4`
>    - `"string"` or `pointer_variable` gonna be the address of it's data

##### Return value

The macro expansion is a function that returns an integer where:
- `0` means failed assertion
- `1` means passed assertion
```c
int value = 2;
if (ACRT_BOOL(&acrt, value)) {
  printf("The assertion with '%d' value is passed    :^)\n", value);
} else {
  printf("The assertion with '%d' value is failed    :^(\n", value);
}
```

##### Panics

When creating an `acrt_t` value with `ACRT_DEFAULT` macro, it's auto-set `on_fail` field to
`ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE`, which means that the current process gonna be exited with
the `1` exit code **at the first failed assertion**. This ensures program validity:

> The next state probably won't be valid if it depends on a failed one.

Change it with the **acrt on fail setter** functions (mentioned at [functions](#functions)
section).

### Functions

The `acrt` library functions can be separated in three distinct groups:
1. [public](#public-functions), which are exposed by the `acrt.h` header file and can be used by
   the final user
2. [semi-public](#semi-public-functions), which are also available at the `acrt.h` header file but
   shouldn't be used
3. [private](#private-functions), which are defined and implemented at local header helpers

#### Public functions

At public functions group, we have:

##### `acrt_display_counting` function

```c
void acrt_display_counting(acrt_t *self, FILE *f);
```

This functions takes an `acrt_t` struct pointer and display the assertion counting table at the
specified `FILE` pointer.

If any of params is `NULL`, the function just returns.

This function requires a `FILE` target since it's also used by `acrt.c` implementors to trigger
text to `stderr`, but you can just call `acrt_display_counting(&acrt, stdout)` and get the normal
behavior too.

##### `acrt_reset_assertions_state` function

```c
void acrt_reset_assertions_state(acrt_t *self);
```

This function takes an `acrt_t` struct pointer and reset assertions related stuff to it's init
state (refers to `counting` and `previous_assertion_failed`).

> [!IMPORTANT]
>
> It doesn't reset other field such as `on_fail` and `context_name`. To do so, consider using other
> setter functions or even the [`ACRT_DEFAULT`](#acrt_default-macro) macro.

##### `acrt_set_context_name_to_custom` function

```c
void acrt_set_context_name_to_custom(acrt_t *self, const char *name);
```

Takes an `acrt_t` struct pointer and sets the `context_name.kind` field to
`CONTEXT_NAME_USE_CUSTOM_NAME`. Then, makes `context_name.data.custom` field points to the `name`
pointer param.

This function will only works if both param aren't `NULL` + `strlen(name)` greater than `0`,
otherwise, nothing is changed.

##### `acrt_set_context_name_to_file` function

```c
void acrt_set_context_name_to_file(acrt_t *self);
```

Takes an `acrt_t` struct pointer and sets the `context_name.kind` to `CONTEXT_NAME_USE_FILE_NAME`.
Note that this function doesn't needs any other param since the `context_name.data.file` is set at
the `acrt_t` initilization (when using [`ACRT_DEFAULT`](#acrt_default-macro) macro).

##### `acrt_set_on_fail_continue_assertions` function

```c
void acrt_set_on_fail_continue_assertions(acrt_t *self);
```

This function takes an `acrt_t` struct pointer and turns the `on_fail.action_kind` field into
`ON_FAIL_CONTINUE_ASSERTIONS`.

##### `acrt_set_on_fail_disable_assertions` function

```c
void acrt_set_on_fail_disable_assertions(acrt_t *self);
```

This function takes an `acrt_t` struct pointer and set the `on_fail.action_kind` field to
`ON_FAIL_DISABLE_ASSERTIONS`.

##### `acrt_set_on_fail_exit_program` function

```c
void acrt_set_on_fail_exit_program(acrt_t *self, int code);
```

This function takes an `acrt_t` struct pointer and set the `on_fail.action_kind` field to
`ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE`. The `code` param is the process exit code after a failed
assertion run.

##### `acrt_set_on_fail_print_counting_and_exit` function

```c
void acrt_set_on_fail_print_counting_and_exit(acrt_t *self, int code);
```

This function takes an `acrt_t` struct pointer and set the `on_fail.action_kind` field to
`ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE`. The `code` param works the same as
mentioned above.

##### `acrt_set_on_fail_run_callback_and_exit` function

```c
void acrt_set_on_fail_run_callback_and_exit(acrt_t *self,
                                            void (*callback)(void *), void *arg,
                                            int code);
```

This function takes an `acrt_t` struct pointer and set the `on_fail.action_kind` field to
`ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE`. The `callback` is the function to be ran and the
`arg` is the param for this same function. The `code` means the exit code after `callback` run on a
failed assertion.

> [!IMPORTANT]
>
> This function requires `callback` to be a non-null pointer, otherwise, it does nothing.

##### `acrt_set_on_fail_run_callback_without_exit` function

```c
void acrt_set_on_fail_run_callback_without_exit(acrt_t *self,
                                                void (*callback)(void *),
                                                void *arg,
                                                int continue_assertions);
```

This function works the same as
[`acrt_set_on_fail_run_callback_and_exit`](#acrt_set_on_fail_run_callback_and_exit-function), but
it doesn't exits the process with an exit code. It alternatively carries a `continue_assertions`
param which is used to decide if remaining assertions should be ran or ignored after a failed one.

##### `acrt_set_display_mode_to_all` function

```c
void acrt_set_display_mode_to_all(acrt_t *self);
```

This function takes an `acrt_t` struct pointer and set the `display_mode` field to
`DISPLAY_MODE_ALL`.

##### `acrt_set_display_mode_to_failed_only` function

```c
void acrt_set_display_mode_to_failed_only(acrt_t *self);
```

This function takes an `acrt_t` struct pointer and set the `display_mode` field to
`DISPLAY_MODE_FAILED_ONLY`.

##### `acrt_set_display_mode_to_quiet` function

```c
void acrt_set_display_mode_to_quiet(acrt_t *self);
```

This function takes an `acrt_t` struct pointer and set the `display_mode` field to
`DISPLAY_MODE_QUIET`.

#### Semi-public functions

Functions that are defined at `acrt.h` header but should only by used by macros's expansion:

##### `__acrt_run_boolean_assertion` function

```c
int __acrt_run_boolean_assertion(acrt_t *self, const unsigned int line,
                                 uintptr_t value, int is_ptr);
```

This function takes an `acrt_t` struct pointer, the source code line of current assertion and the
`value` to be asserted.

The `value` param must be an `uintptr_t` type to be later converted into an actual `int` or
`void *` depending on the `is_ptr` param. This definition allows source code to be compiled without
warnings since this function is used within the [`ACRT_BOOL`](#acrt_bool-macro) macro's expansion
which can receive:
- numeric literals
- numeric variables
- pointers

#### Private functions

Functions that are defined at helper headers (non `acrt.h` file):

##### `display_acrt_result` function at `acrt_display.h`

```c
void display_acrt_result(acrt_result_t *res);
```

This function takes an [`acrt_result_t`](#acrt-result-struct) struct pointer and prints it's data.
The file target (`stdout` / `stderr`) is decided by the `res` inner fields + function helpers.

Does nothing if the `res` is `NULL`.

##### `display_counting_data` function at `acrt_display.h`

```c
void display_counting_data(const char *ctx_name,
                           struct __acrt_counting *counting, FILE *f);
```

This function takes a context name, a `counting` pointer and a target `FILE` to display it's data.

Does nothing if the `counting` or `f` param is `NULL`.

##### `acrt_result_is_passed` function at `acrt_result.h`

```c
int acrt_result_is_passed(acrt_result_t *self);
```

This function takes an [`acrt_result_t`](#acrt-result-struct) struct pointer and returns `1` if
status somehow refers to **passed** (even when with warnings).

If `self` is `NULL`, returns `0`.

##### `acrt_result_new_bool` function at `acrt_result.h`

```c
acrt_result_t acrt_result_new_bool(const char *name, const unsigned int line,
                                   uintptr_t value, int is_ptr);
```

Creates a new [`acrt_result_t`](#acrt-result-struct) struct with `boolean` kind, the inner kind
(`pointer` or `integer`), data and status are defined at function impl (`acrt_result.c`) based on
`value` and `is_ptr` params.

## Unit tests

The source code testing was done entirely in C + Python automation.

You can check all the tests at [tests](./tests/) directory.

They include:
1. the header file for the testing context
  - function and type _(definitions)_ that should be tested
2. the local util header file ([test_util.h](./tests/test_util.h))
  - set of macros for testing run + fail identifying
    - line number
    - context fail

### Test building

The [Makefile](./Makefile) provide shorthand for exe compiling, just run:
```sh
make build-tests
```

All the files at [tests](./tests/) directory will be compiled separately and be placed at
`out/test-<ORIGINAL_NAME>.exe` output.

> [!NOTE]
>
> The `.exe` extension is used to guarantee compatibility between Posix and Windows systems.

### Test running

The test files can be ran separately by just calling the file path (like
`./out/test-acrt_default.exe`).

If the testing succeeds, nothing is print and `0` exit code is returned, otherwise, an error
message is displayed and `1` exit code is returned.

You can also call the [run_tests.py](./run_tests.py) automation (with Python runtime such as
[python](https://www.python.org/), [uv](https://docs.astral.sh/uv/), ...) but it requires all test
files already compiled to the expected output.

## Usage

`acrt` library provides a common type for assertion run:
```c
typedef struct { ... } acrt_t;
```

It holds:
- [context name](#context-name) used when displaying info at output
- [failure behavior](#failure-behavior) enumerator + data pointers
- [display mode](#display-mode) _(when the data is displayed)_
- [counting object](#counting-object) _(storing assertions ran and it's status)_

> [!NOTE]
>
> Content bellow just shows fields description. Go to [assertions](#assertions) to actual usage
> help.

### Context name

Context name refers to the context where `acrt_t` is located:
```c
struct {
  enum { ... } kind;
  union { const char *file, *custom; } data;
} context_name;
```

The context name is used as _'scope identifier'_ for the created `acrt_t` and displayed with the
assertion info (if needed). This helps to use and differ multiple `acrt_t`s, even when at same
scope.

All these fields are filled within `ACRT_DEFAULT` macro (take a look at [assertions](#assertions)
section). `custom` field will point to the **default context name** string (doesn't matter since
kind refers to `CONTEXT_NAME_USE_FILE_NAME` variant, which means that the displayed context will be
`file` field by default).

You still can change the context variant (or use a custom one). Read
[context name setters](#context-name-setters).

### Failure behavior

Failure behavior is controlled through the `on_fail` field.
```c
struct {
  enum  { ... } action_kind;
  union { ... } data;
} on_fail;
```

`action_kind` refers to which action will be executed and `data` stores the values used across
these actions.

There's six different behavior options:
1. `ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE` which takes the `data.code` integer value and use it as
   exit code for the running process
2. `ON_FAIL_DISABLE_ASSERTIONS` which disable all following assertions for the **self `acrt_t`
   object**
3. `ON_FAIL_CONTINUE_ASSERTIONS` which still continues the program normally
4. `ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE` which prints the assertion counting
   table at `stderr` and exits the running process with the `data.print_counting_and_exit.code`
   exit code
5. `ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE` which runs the
   `data.run_callback_and_exit.callback` function pointer passing the
   `data.run_callback_and_exit.arg` as param and exiting the running process with the
   `data.run_callback_and_exit.code` value as exit code
6. `ON_FAIL_RUN_CALLBACK_AND_DO_NOT_EXIT` does the same as variant above but without exiting the
   running process. It also carries the `continue_assertions` that decides if following assertions
   gonna be ran or ignored after callback

They all can be set by using the [on fail setters](#on-fail-setters) functions.

### Display mode

Display mode is an anonymous enum and can be separated in three different variants:
1. `DISPLAY_MODE_FAILED_ONLY` means that **only failed assertions should be displayed**
2. `DISPLAY_MODE_ALL` means that **all assertions should be displayed**
3. `DISPLAY_MODE_QUIET` means that **no assertions should be displayed**

You can switch current mode by using the [display mode setters](#display-mode-setters).

### Counting object

The `acrt` struct holds a counting object that is updated whenever an assertion is called (and
update is required). It can also be reset to an initial state by using the
[reset state setter](#reset-state-setter).

### Assertions

Here's a simple example for boolean assertions:
```c
int main() {
  acrt_t acrt = ACRT_DEFAULT;
  ACRT_BOOL(&acrt, 1);
  ACRT_BOOL(&acrt, "char pointer");
  ACRT_BOOL(&acrt, 2 < 3);
  ...
}
```

First, we need to create a new `acrt_t` instance by with the `ACRT_DEFAULT` macro. It returns an
`acrt` default state directly in place.

The first param must be an non-null `acrt_t` pointer, otherwise the program does nothing and the
call returns `0`. The second param can be any _bool compatible_ value:
- integer or float literals/variables
- pointers
- expressions
- ...

#### Return value

The assertion macro call returns an integer where:
- `0` means assertion failure
- `1` means assertion success

It can be used as control flow:
```c
if (ACRT_BOOL(&acrt, heavy_pointer)) {
  deinit_heavy(heavy_pointer);
  free(heavy_pointer);
}
```

### Context name setters

Two different context name setters are provided by the API:
```c
// Sets context name kind to custom + takes custom name for current context. Only works when both
// pointers are non-null + `strlen(name)` > 0.
void acrt_set_context_name_to_custom(acrt_t *self, const char *name);

// Sets context name kind to file.
void acrt_set_context_name_to_file(acrt_t *self);
```

### On fail setters

Six different `on_fail` setters are provided by the API:
```c
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
```

### Display mode setters

Three different display mode setters are provided by the API:
```c
// Set the acrt display mode field to 'DISPLAY_MODE_ALL' variant.
void acrt_set_display_mode_to_all(acrt_t *self);

// Set the acrt display mode field to 'DISPLAY_MODE_FAILED_ONLY' variant.
void acrt_set_display_mode_to_failed_only(acrt_t *self);

// Set the acrt display mode field to 'DISPLAY_MODE_QUIET' variant.
void acrt_set_display_mode_to_quiet(acrt_t *self);
```

### Reset state setter

The `acrt_t` inner state (counting and fail trigger) can be reset by using this function:
```c
// Reset the assertions counting and previous_assertion_failed fields only.
void acrt_reset_assertions_state(acrt_t *self);
```

### Counting display

The counting table can be displayed manually by using this function:
```c
// Display the inner counting data. This function requires an acrt pointer + a
// file target to diplay (stdout/stderr).
//
// Does nothing if any of param is NULL.
void acrt_display_counting(acrt_t *self, FILE *f);
```
