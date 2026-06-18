# Usage

This page provides usage help and examples (strongly based on [examples](./examples) files):

- [Basic](#basics)

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
