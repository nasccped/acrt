# `acrt` tests

These files tests if acrt builder/setters are working properly. You can build them using the
`make build-tests` provided by the root [Makefile](../Makefile).

Since the acrt requires to work properly to, well... assert, the major part of code right there
contains it's own assertion logic provided by the self file scope functions and the
[test_util](./test_util.h) header.
