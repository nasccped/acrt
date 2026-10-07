<div align="center">

# so-handling branch

</div>

The only purpose of this branch is to test and understand the `.ELF` parsing and reading strategy.
My thoughts and discoveries gonna be documented right bellow!

## Table of contents

- [the program's goal](#the-programs-goal)
  - [why does it matter?](#why-does-it-matter)
  - [the `acrt`'s features](#the-acrts-features)
- [resources](#resources)

## The program's goal

To talk about the `.so` file handling, I need to mention the program's goal!

The `acrt` program was designed to handle tiny test cases in C to check if your code is doing well.
I thought about turning C testing kinda simpler and delegating all the cases to an specified
controller!

TL;DR, the common testing implementation could be enforced by the compiler + preprocessing:

```c
#include <acrt.h>

TEST_CASE(test_mul) {
  ACRT_BOOL((2 * 2) > 3);
}

TEST_CASE(test_div) {
  ACRT_BOOL((2 / 2) == 1);
}
```

Instead of trivial copy, paste and flow managing:

```c
#include <stdio.h>
#include <assert.h>

int main(void) {
  int passed_count = 0;

  printf("testing mul.\n");
  assert((2 * 2) > 3);
  printf("passed!\n");
  passed_count++;

  printf("testing div.\n");
  assert((2 / 2) == 1);
  printf("passed!\n");
  passed_count++;

  printf("Tests passed: %d\n", passed_count);
  return 0;
}
```

### Why does it matter?

Test is a crucial part of software development. It purposes is to guarantee that what we've write
not only compiles but also does what it was expected to do!

Since C is a relatively low-level programming language, it doesn't provides extreme kind of
abstraction, forcing us to write long blocks of code which sometimes:
- isn't meaningful / easy-readable;
- repetitive _(avoidable by using **preprocessor**)_;
- is humanly _error-prone_ (commonly caused by the two topics above)

Passing the test responsibility to a dedicated entity can turn the test writing a more safe and
pleasurable experience.

### The `acrt`'s features

Here are the `acrt`'s main features:
- provide a meaningful test syntax + still valid C code:
  - easily achieved by using [C preprocessor macros](https://gcc.gnu.org/onlinedocs/cpp/Macros.html);
  - easy-readable by the developer + still valid for the compiler;
- take test cases as input and, well, run them.

To do so, the `acrt` framework must be separated in two different instances:
1. A library which provides the testing _syntax-sugar_;
2. A program _(binary)_ which works as runtime manager for the test cases input (mentioned at the
   previous list).

This documentation talks about (mainly) the test cases handling strategies.

## Resources

Resources that helped me during development/documentation:
- [how to execute an object file](https://blog.cloudflare.com/how-to-execute-an-object-file-part-1/)
  at Cloudflare (by [Ignat Korchagin](https://blog.cloudflare.com/author/ignat/));
- `elf` manual pages disposed by my wsl (also available
  [here](https://man7.org/linux/man-pages/man5/elf.5.html));
- `dlinfo` manual pages disposed by my wsl (also available
  [here](https://www.man7.org/linux/man-pages/man3/dlinfo.3.html))
- `dl_iterate_phdr` manual pages disposed by my wsl (also available
  [here](https://man7.org/linux/man-pages/man3/dl_iterate_phdr.3.html)).
