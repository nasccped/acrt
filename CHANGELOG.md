# Changelog

acrt library changelog.

## v0.0.10

- docs:
  - add better api description
- tests:
  - test invalid argument setter fail
  - **(refactor)** replace two macro calls by a single one (`test_util.h`)
  - test result status (if is warning)
- refactor:
  - replace two _dunder-private_ functions by a single one (`acrt.(c/h)`)
  - replace two _equal-meaning_ constructor functions by a single one (`acrt_result.(c/h)`)

## v0.0.9

- replace `ACRT_NEW` macro by `ACRT_DEFAULT`
  - placeholder for default instance instead of function call allowing global scope static
    variables
  - `function context name` field and setter must be remove to do this work `:^(`
  - update [DOCS](./DOCS.md) [README](./README.md) samples, tests and examples due to change
    mentioned above

## v0.0.8

- docs update
  - update README to fit
    [x] requirements
    [x] set up
    [x] usage
    [x] new anchors
  - update(move) usage to general docs

## v0.0.7

- add github workflow
  - auto generate source code tar file when pushing tags
  - auto generate release notes page when pushing tags

## v0.0.6

- add license description at the top of source code
- allow variable usage as boolean value

## v0.0.5

- add a better test coverage
- logic refactor
- better api meaning
  - setter functions
  - private implementation by using anonymous fields
- remove outdated examples
  - [ ] **todo:** replace it by new ones

## v0.0.4

- return integer as assertion result

## v0.0.3

- docs:
  - cover inner values handling
  - cover boolean/expression assertions

## v0.0.2

- docs: a better documentation coverage
  - added library set up
  - added library usage
- refactor:
  - use a better code workflow (`src/acrt.c`)
- tests:
  - add readme samples to test coverage
  - add `default_values` mentioned at [usage](./USAGE.md) page to test coverage

## v0.0.1 first patch release

- basic boolean assertion (works for bool-like data such as integers, pointer, etc)
