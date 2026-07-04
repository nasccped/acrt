#ifndef _ACRT_RESULT_H_
#define _ACRT_RESULT_H_

// Super type (tagged enum) that holds the inner assertion state such as:
// - context (name, line number)
// - kind (boolean, eq, not_eq, ...)
// - value (pointer, pointer pair, ...)
// - status (passed or not)
struct __acrt_result {

  // Assertion context fields.
  struct {

    // Context name being used.
    const char *name;

    // Current line number.
    unsigned int line;

  } context;

  // Kind of assertion result.
  enum {

    // When the value being asserted is just a 'bool' compatible value (like
    // int, float, ...).
    SIMPLE_BOOLEAN_ASSERTION_KIND,

    // When the value being asserted is a pointer.
    POINTER_BOOLEAN_ASSERTION_KIND,

  } kind;

  // Context data held by the assertion. Availability according to 'kind'
  // variant.
  union {

    // Holds pointer address when 'kind == POINTER_BOOLEAN_ASSERTION_KIND'.
    void *single_pointer;

  } data;

  // If the assertion was (or not) passed.
  int passed;
};

// Creates a new acrt_result from a boolean value
// ('NUMBER_BOOLEAN_ASSERTION_KIND').
struct __acrt_result acrt_result_from_int(const char *name,
                                          const unsigned int line, int value);

// Creates a new acrt_result from a pointer ('POINTER_BOOLEAN_ASSERTION_KIND' by
// default).
struct __acrt_result acrt_result_from_single_pointer(const char *name,
                                                     const unsigned int line,
                                                     void *pointer);

#endif
