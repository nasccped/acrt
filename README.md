<div align="center">

# acrt

![c library: minimal](https://img.shields.io/badge/c_library-minimal-blue)
![cargo inspired](https://img.shields.io/badge/cargo_inspired-grey?logo=rust)
![current tag](https://img.shields.io/github/v/tag/nasccped/acrt)
![license: MIT](https://img.shields.io/github/license/nasccped/acrt)

</div>

This is **acrt**, a C testing library.

The major purpose is to provided pretty printing and good control of assertions (unit testing)
using no thirdparty frameworks or other complex stuff.

This library is under development and breaking changes aren't expected ~(but strongly possible)~.

## Set up on machine

To get and use this library, you'll need:
- `git` program
- `gcc` program

- clone the repository:
```sh
git clone https://github.com/nasccped/acrt
```
- copy only the necessary stuff and then delete:
```sh
cp acrt/src path/to/your_code/acrt -r && rm -rf acrt;
cd path/to/your_code
```
- compile (with all `c` files) and run:
```sh
gcc $(find acrt/*.c) your_file.c -o your_output && your_output;
```

> [!NOTE]
>
> 1. Mentioned code above is `sh` compatible. Consider using a Makefile for 'auto-building'.
> 2. There's also local examples and tests which you can run (requires `make`):
>    ```sh
>    make build-example;
>    make build-test;
>    ```
>    Binaries will be droped at `out` directory with `exmp`/`test` prefix (ends with `exe` extension
>    for Windows port).

## Usage

You can use the library with the `acrt_t` struct and it's macros:

```c
acrt_t acrt = acrt_new();

// boolean assertions
acrt_bool(acrt, 1);
acrt_bool(acrt, 2 != 0);

char *s = "hello";
// pointer assertions
acrt_bool(acrt, s);
s = NULL;
acrt_bool(acrt, !s);
```

That was a short overview. Take a look at [usage](./USAGE.md) page for more details.

## License

This project is under the [MIT](./LICENSE) license!
