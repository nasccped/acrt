# acrt library documentation

This page provides the `acrt` library code description as a whole. It covers the functions, macros
and common types defined within the library source file. You can read it all the way or just use
the following **table of contents**:

- [about the library](#about-the-library)
  - [structs](#structs)
    - [`acrt`](#acrt-struct)
      - [`context_name`](#context_name-field)
      - [`counting`](#counting-field)
      - [`on_fail`](#on_fail-field)
      - [`display_mode`](#display_mode-field)
      - [`counting`](#counting-field)
      - [`previous_assertion_failed`](#previous_assertion_failed-field)
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
- [testing](#testing)
  - [building](#test-building)
  - [running](#test-running)

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

##### `context_name` field

The context name stores the name held by the `acrt` object, such as:
- `kind` on naming being used (`CONTEXT_NAME_USE_FILE_NAME` or `CONTEXT_NAME_USE_CUSTOM_NAME`)
- `data.file` storing the `__FILE__` string value at `acrt` initialization
- `data.custom` storing a custom string defined by the user

This name is displayed at `stdout` / `stderr` followed by the assertion info, i. e:
```txt
This:
100 |
101 | ASSERT_BOOL(&acrt, NULL);
102 |

Prints this message:
    |
    | [<CONTEXT_NAME>: 101] boolean assertion failed
    |                       value points to 0000000000000000.
    |
```

By default (when using [`ACRT_DEFAULT`](#acrt_default-macro) macro), `<CONTEXT_NAME>` will refers
to `__FILE__` (file name of `acrt` initialization), but you can set a custom name by using the
[`acrt_set_context_name_to_custom`](#acrt_set_context_name_to_custom-function) function.

##### `counting` field

The `counting` is a struct ([`__acrt_counting`](#acrt-counting-struct)) that refers to an assertion
counting. It stores:
- total of assertions
- total of passed assertions _(with(out) warnings)_
- total of failed assertions
- total of ignored assertions

It's major purpose is to print a **counting table** to the output whenever necessary.

##### `on_fail` field

```c
struct {
  enum  { ... } action_kind;
  union { ... } data;
} on_fail;
```

The `on_fail` field is a tagged union where the `action_kind` refers to the action to be
performed while `data` stores the actual data to be used by this action.

When initializing a new `acrt_t` struct with the [`ACRT_DEFAULT`](#acrt_default-macro) macro, it
comes with `{ .action_kind = ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE, .data.code = 1 }` but it can be
changed through the `acrt_set_on_fail_<ACTION>` functions. Take a look at the
[table of contents](#).

The available options for `on_fail` field are:
- `ON_FAIL_EXIT_PROGRAM_WITH_EXIT_CODE` which exits the current process at the next failed
  assertion
  - `data.code` is the value used as exit code (`int`)
- `ON_FAIL_DISABLE_ASSERTIONS` which disable all following assertions after a failed one and
  increments the [`counting.ignored`](#counting-field) field
- `ON_FAIL_CONTINUE_ASSERTIONS` which continues all assertions normally (even after a failed one)
- `ON_FAIL_PRINT_COUNTING_AND_EXIT_PROGRAM_WITH_EXIT_CODE` which prints the counting table (refers
  to [`counting`](#acrt-counting-struct) struct) and exit the current process at the next failed
  assertion
  - `data.print_counting_and_exit.counting` is used as `counting` pointer when doing print stuff
  - `data.print_counting_and_exit.code` is the value used as exit code (`int`)
- `ON_FAIL_RUN_CALLBACK_AND_EXIT_WITH_EXIT_CODE` which runs a callback function pointer with an
  optional argument, and then, exits the current process after a failed assertion
  - `data.run_callback_and_exit.callback` is the function pointer used after the failed assertion
  - `data.run_callback_and_exit.arg` is the param passed to the function mentioned above
  - `data.run_callback_and_exit.code` is the exit code for the current process
- `ON_FAIL_RUN_CALLBACK_AND_DO_NOT_EXIT` which does almost the same as the option above, but it
  doesn't exits the current process
  - `data.run_callback_and_do_not_exit.continue_assertions` is used to decide if following
    assertions should be ignored or not after a failed one (working similar to `disable / continue`
    options)

##### `display_mode` field

The `display_mode` field is a enum used to decide if an [`assertion result`](#acrt-result-struct)
should (or not) be displayed. The default variant (when using [`ACRT_DEFAULT`](#acrt_default-macro)
macro) is `DISPLAY_MODE_FAILED_ONLY` but it can be changed by an `acrt_set_display_mode_to_<MODE>`
function (mentioned at [table of contents](#)).

The available variants means:
- `DISPLAY_MODE_FAILED_ONLY`: display only failed assertion(s)
- `DISPLAY_MODE_ALL`: display all kind of assertions, even when passed
- `DISPLAY_MODE_QUIET`: don't display any assertions, even when failed

##### `counting` field

The `counting` field refers to the [`counting`](#acrt-counting-struct) struct stored by the
`acrt_t` item. Now, each `acrt_t` contains is _own counter_. This field is also updated at runtime
by the local scoped `acrt.c`'s functions.

##### `previous_assertion_failed` field

`previous_assertion_failed` is just a boolean field that stores if the latest assertion was failed.
It's used to decide if following assertions should (or not) be ran (with
[`on_fail`](#on_fail-field) field, btw).

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

## Testing

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
