/* ****************************************************************************
 * MIT License
 *
 * Copyright (c) 2026 nasccped
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ************************************************************************* */

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
