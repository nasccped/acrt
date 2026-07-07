#ifndef _ACRT_RESULT_H_
#define _ACRT_RESULT_H_

// Super type (tagged enum) that holds the inner assertion state such as:
// - context (name, line number)
// - kind (boolean, eq, not_eq, ...)
// - value (pointer, pointer pair, ...)
// - status (passed or not)
typedef struct {

  // Assertion context fields.
  struct {

    // Context name being used.
    const char *name;

    // Current line number.
    unsigned int line;

  } context;

  // Kind of assertion.
  enum {

    // When asserting normal integer values.
    INTEGER_BOOLEAN_ASSERTION_KIND,

    // When the value being asserted is a pointer.
    POINTER_BOOLEAN_ASSERTION_KIND,

  } kind;

  // Context data held by the assertion. Availability according to 'kind'
  // variant.
  union {

    // Holds pointer address when 'kind == POINTER_BOOLEAN_ASSERTION_KIND'.
    void *boolean_pointer;

  } data;

  // Result status for the current assertion.
  enum {

    // When the assertion was successfully passed without any warning.
    PASSED_ASSERTION_WITHOUT_WARNING,

    // When the assertion was passed but with some warning.
    PASSED_ASSERTION_WITH_WARNING,

    // When the assertion was failed.
    FAILED_ASSERTION,

    // When the assertion was ignored.
    IGNORED_ASSERTION

  } status;

} acrt_result_t;

// When the result fields doesn't matter (just looking for ignored status).
#define IGNORED_ASSERTION_RESULT                                               \
  (acrt_result_t) { .status = IGNORED_ASSERTION }

// Returns 1 if self is non-null + inner status is recognized as passed value,
// otherwise, returns zero.
int acrt_result_is_passed(acrt_result_t *self);

// Creates a new acrt_result from a int value
// ('INTEGER_BOOLEAN_ASSERTION_KIND').
acrt_result_t acrt_result_from_int(const char *name, const unsigned int line,
                                   int value);

// Creates a new acrt_result from a pointer ('POINTER_BOOLEAN_ASSERTION_KIND' by
// default).
acrt_result_t acrt_result_from_single_pointer(const char *name,
                                              const unsigned int line,
                                              void *pointer);

#endif
