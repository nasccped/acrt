# acrt library documentation

This page provides the `acrt` library code description as a whole. It covers the functions, macros
and common types defined within the library source file. You can read it all the way or just use
the following **table of contents**:

- [about the library](#about-the-library)
  - [structs](#structs)
    - [`acrt`](#acrt-struct)
    - [`acrt counting`](#acrt-counting-struct)
    - [`acrt result`](#acrt-result-struct)

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
