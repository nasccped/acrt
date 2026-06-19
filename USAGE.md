# Usage

This page provides usage help and examples (strongly based on [examples](./examples) files):

- [Basic](#basics)
- [Inner values](#inner-values)

## Basics

The `acrt` library 101 (at [basics](./examples/01-basics.c) example).

### Including

`acrt` is a libray packed into a single header, the `acrt.h`. This approach allows the features
usage without include hell concern:

> - I need the `int_type`, so I'll include the `my_int.h`
> - I need the `string_type`, so I'll include the `my_str.h`

Just include the `acrt.h` and it's done!
```c
#include "path/to/acrt.h"

// code ...
```

> [!IMPORTANT]
>
> Even if including only `acrt.h`, passing c files to the compiler is required since they hold all
> functions implementation:
> ```sh
> # this fails
> gcc acrt.c my_main.c -o result.exe;
> # this works
> gcc acrt.c assertion_counter.c assertion_display.c assertion_wrapper.c my_main.c -o result.exe
> ```
>
> Consider building a `Makefile` or a `nob.h` for your project.

### The `acrt_t` and it's macros

All assertion stuff is done through an `acrt_t` object. There's no need to build different assert
objects for different kind of assertions, just use a single `acrt_t` and run assertions over it:

#### Initializing

Since `acrt_t` is a struct, it can be initialized with the zero initializer (example bellow):
```c
// works but not recommended
acrt_t acrt = {0};
```

Even if possible, that isn't a recomended approach. The `acrt_t` type holds a lot of enums, which
are evaluated to the first variant when calling the _zero initializer_ and can lead to runtime
unexpected behavior (considering the next releases changes).

You must use the `acrt_new` function which does the initialization with all the expected values:
```c
acrt_t acrt = acrt_new();
```

#### Macros usage

The acrt usage carries a lot of debug data such as file name, line number, expression, etc...

To avoid this boilerplate/copypaste code, the `acrt.h` provides macros which automatically gets all
this stuff and reaches a faster workflow:
```c
acrt_macro(acrt, /* other params... */);
```

Since macro is code replace by the preproc, you shouldn't pass the struct pointer to it, but the
struct value instead:
```c
// compile fail if references:
acrt_macro(&acrt);

// compile fail if pointers:
acrt_t *pointer = malloc(sizeof(acrt_t));
acrt_macro(pointer);

// works well
acrt_macro(acrt);
```

> [!NOTE]
>
> All the macros are mentioned at this page, consider reading the `table of contents` at
> [usage](#usage) section.

#### Don't free

Since `acrt_new` returns a struct literal (which also holds only `const pointers`), there's no need
to use `free` on it or any of it's inner fields _(on the current version, actually)_.

## Inner values

> Examples at [02-inner_values.c](./examples/02-inner_values.c).

The `acrt_t` contains inner values that are checked and updated at runtime. You can change it
manually by the `struct_variable.field = ...` syntax but this can lead to inconsistent behavior.
Use the api functions instead:

### Group name

When doing assertions, the process can be display at `stdout`/`stderr` (more details at
[printing enums](#printing-enums)). The content will looks like this:

```txt
[<GROUP_NAME> - <FILE_NAME>: <LINE_NUMBER>]: <MACRO_CALL_DESCRIPTION>
                                             <OPTIONAL_VALUES_DESCRIPTION>
```

All these fields are picked during macro call (so, they're immutable). But the `<GROUP_NAME>`
refers to the `acrt_t` context, like:

- `acrt_bool(acrt, is_prime(2))`
- `acrt_bool(acrt, is_palindrome("natan"))`
- `...`

So, it would be nice if you could change the assertion context name.

To do so, you can use the `acrt_set_name` function which takes a pointer to the `acrt_t` struct and
a string literal:
```c
acrt_set_name(&acrt, "custom name");
```

This function gonna fail if any of pointer equals to NULL or string length is equals to zero. The
default group name is `ACRT UNNAMED`.

### Printing enums

By default, the `acrt_t` comes with `display_mode = DISPLAY_ERRORS_ONLY`. So, all success
assertions won't be displayed unless you specify to do so using the `acrt_set_display_variant`
function:
```c
acrt_set_display_variant(&acrt, <VARIANT>);
```

Available variants:
- `DISPLAY_ERRORS_ONLY`
- `DISPLAY_ALL_ASSERTIONS` _(even when success)_
- `NO_ASSERTION_DISPLAY` _(even if error)_

### Exit process

By default, the `acrt_t` forces the program exit when reaching a failed assertion. You can change
it by using the `acrt_set_on_fail` function:
```c
acrt_set_on_fail(&acrt, <VARIANT>);
```
Available variants:
- `EXIT_PROGRAM` _(with error status code - `1`)_
- `SKIP_ASSERTIONS` _(skip all other assertions)_
- `CONTINUE_ASSERTIONS` _(continues the program normally)_

### Usage tip

> [!TIP]
>
> If a very custom `acrt_t` is required, consider packing it within a function:
> ```c
> acrt_t my_acrt() {
>   acrt_t acrt = acrt_new();
>   acrt_set_name(&acrt, "my custom name");
>   acrt_set_display_variant(&acrt, DISPLAY_ALL_ASSERTIONS);
>   acrt_set_on_fail(&acrt, CONTINUE_ASSERTIONS);
>   return acrt;
> }
> ```
