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

// Context block.
#define CTX(NAME, LINE) {.name = (NAME), .line = (LINE)}

int acrt_result_is_passed(acrt_result_t *self) {
  if (!self)
    return 0;

  return self->status == PASSED_ASSERTION_WITHOUT_WARNING ||
         self->status == PASSED_ASSERTION_WITH_WARNING;
}

acrt_result_t acrt_result_from_int(const char *name, const unsigned int line,
                                   int value) {
  return (acrt_result_t){.context = CTX(name, line),
                         .kind = INTEGER_BOOLEAN_ASSERTION_KIND,
                         .status = value ? PASSED_ASSERTION_WITHOUT_WARNING
                                         : FAILED_ASSERTION};
}

acrt_result_t acrt_result_from_single_pointer(const char *name,
                                              const unsigned int line,
                                              void *pointer) {
  return (acrt_result_t){.context = {.name = name, .line = line},
                         .kind = POINTER_BOOLEAN_ASSERTION_KIND,
                         .data.boolean_pointer = pointer,
                         .status = pointer ? PASSED_ASSERTION_WITHOUT_WARNING
                                           : FAILED_ASSERTION};
}
