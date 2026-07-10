# acrt library documentation

- [Unit tests](#unit-tests)
  - [Test building](#test-building)
  - [Test running](#test-running)
- [Usage](#usage)

## Unit tests

The source code testing was done entirely in C + Python automation.

You can check all the tests at [tests](./tests/) directory.

They include:
1. the header file for the testing context
  - function and type _(definitions)_ that should be tested
2. the local util header file ([tes_util.h](./tests/test_util.h))
  - set of macros for testing run + fail indentifying
    - line number
    - context fail

### Test building

The [Makefile](./Makefile) provide shorthands for exe compiling, just run:
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

# Usage

> TODO:
>
> usage doc update due to latest breaking changes!
