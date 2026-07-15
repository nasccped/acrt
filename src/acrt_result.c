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

#include "acrt_result.h"

// Creates a new result struct but with only context.name and context.line
// fields set.
#define WITH_CTX_ONLY(NAME, LINE)                                              \
  (acrt_result_t) {                                                            \
    .context = {.name = (NAME), .line = (LINE) }                               \
  }

int acrt_result_is_passed(acrt_result_t *self) {
  if (!self)
    return 0;

  switch (self->status) {

    // passed conditions
  case PASSED_ASSERTION_WITHOUT_WARNING:
  case PASSED_ASSERTION_WITH_WARNING:
    return 1;

    // failed conditions
  case FAILED_ASSERTION:
  case IGNORED_ASSERTION:
    break;
  }

  return 0;
}

acrt_result_t acrt_result_new_bool(const char *name, const unsigned int line,
                                   uintptr_t value, int is_ptr) {
  acrt_result_t result = {.context = {.name = name, .line = line},
                          .status = value ? PASSED_ASSERTION_WITHOUT_WARNING
                                          : FAILED_ASSERTION};
  if (is_ptr) {
    result.kind = POINTER_BOOLEAN_ASSERTION_KIND;
    result.data.single_pointer = (void *)value;
  } else {
    result.kind = INTEGER_BOOLEAN_ASSERTION_KIND;
    result.data.integer_cast = (int)value;
  }
  return result;
}
