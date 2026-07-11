<div align="center">

# acrt

![c library: minimal](https://img.shields.io/badge/c_library-minimal-blue)
![cargo inspired](https://img.shields.io/badge/cargo_inspired-grey?logo=rust)
![current tag](https://img.shields.io/github/v/tag/nasccped/acrt)
![license: MIT](https://img.shields.io/github/license/nasccped/acrt)

</div>

**acrt** is a C testing library that provides `<assert.h>`'s header similar features and was
named with it's greatest api purpose: _assertions_.

Inspired by [rust](https://doc.rust-lang.org/rust-by-example/testing/unit_testing.html) and
[go lang](https://go.dev/doc/tutorial/add-a-test) testing environment, it runs assertions the same
as other libraries but with a more detailed overview, strong behavior control and simple
implementation.

> [!NOTE]
>
> 1. This library is under development and made entirely with my academic/amateur knowledge. It was
>    just a personal project and shouldn't be taken seriously.
> 2. Using it professionally is strongly not recomended.
> 3. breaking changes and weak retro-compatibility are possible, btw...

## Requirements

This project requires:
- a C compiler _([gcc](https://gcc.gnu.org/) preferable)_
- [glibc](https://www.gnu.org/software/libc/) compatible implementor

Install [mingw](https://sourceforge.net/projects/mingw) if on windows.

> [!TIP]
>
> [make](https://www.gnu.org/software/make/) and [python](https://www.python.org/) commands were
> used during development. They can turn the testing and installing processes into a less painful
> experience.

## Setting up

You can download and use the source code directly from release packages:

If you're at Unix system, just run:
```sh
curl -O -L https://github.com/nasccped/acrt/releases/latest/download/acrt-lib.tar.gz && \
  tar -xzf acrt-lib.tar.gz && \
  rm acrt-lib.tar.gz
```

If you're at Windows Powershell, run this instead:
```powershell
Invoke-WebRequest `
  -Uri "https://github.com/nasccped/acrt/releases/latest/download/acrt-lib.tar.gz" `
  -OutFile "acrt-lib.tar.gz"; `
tar -xzf acrt-lib.tar.gz; `
rm acrt-lib.tar.gz;
```

You can also get, use and experiment it by using [git](https://git-scm.com/), of course:
```sh
git clone https://github.com/nasccped/acrt && cd acrt
```

_Take a look at [DOCS](./DOCS.md) page for testing and other tips..._

## Usage

A simple usage demo:
```c
acrt_t acrt = ACRT_NEW();

// bool assertion
ACRT_BOOL(&acrt, 2 > 1);
ACRT_BOOL(&acrt, 0.1 + 0.2 != 0.3);

// pointer assertions
char *s = "hello";
ACRT_BOOL(&acrt, s);
s = NULL;
ACRT_BOOL(&acrt, !s);
```

Other demos and usage tips can be found at [examples](./examples) and [DOCS](./DOCS.md),
respectively.

## License

This project is under the [MIT](./LICENSE) license!
