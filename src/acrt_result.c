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

char *acrt_result_get_kind_as_str(acrt_result_t *self) {
  if (!self)
    return NULL;

  switch (self->kind) {
  case IGNORED_ASSERTION_KIND:
    return "ignored";

  case BOOLEAN_ASSERTION_KIND:
    return "boolean";

  case EQ_ASSERTION_KIND:
    return "eq";
  }

  return "undefined";
}

int acrt_result_is_ignored(acrt_result_t *self) {
  return self ? self->kind == IGNORED_ASSERTION_KIND : 0;
}

int acrt_result_is_passed(acrt_result_t *self) {
  if (!self)
    return 0;

  switch (self->kind) {
  case IGNORED_ASSERTION_KIND:
    return 0;

  case BOOLEAN_ASSERTION_KIND:
    return self->data.boolean_assertion.kind == POINTER_VALUE_KIND
               ? (self->data.boolean_assertion.value.pointer ? 1 : 0)
           : self->data.boolean_assertion.value.integer ? 1
                                                        : 0;

  case EQ_ASSERTION_KIND:
    return self->data.eq_assertion.result;
  }

  return 0;
}

int acrt_result_is_warning(acrt_result_t *self) {
  if (!self)
    return 0;

  switch (self->kind) {
  case IGNORED_ASSERTION_KIND:
  case BOOLEAN_ASSERTION_KIND:
    return 0;

  case EQ_ASSERTION_KIND:
    return self->data.eq_assertion.warning != NO_WARNING;
  }

  return 0;
}

acrt_result_t acrt_result_new_bool(const char *name, const unsigned int line,
                                   intptr_t value, int is_ptr) {
  acrt_result_t result = WITH_CTX_ONLY(name, line);
  result.kind = BOOLEAN_ASSERTION_KIND;

  if (is_ptr) {
    result.data.boolean_assertion.kind = POINTER_VALUE_KIND;
    result.data.boolean_assertion.value.pointer = (void *)value;
  } else {
    result.data.boolean_assertion.kind = INTEGER_VALUE_KIND;
    result.data.boolean_assertion.value.integer = (int)value;
  }

  return result;
}

acrt_result_t acrt_result_new_eq(const char *name, const unsigned int line,
                                 void *left, void *right,
                                 int (*eq_func)(void *, void *)) {
  acrt_result_t result = WITH_CTX_ONLY(name, line);
  int temp;

  result.kind = EQ_ASSERTION_KIND;
  result.data.eq_assertion.params.left = left;
  result.data.eq_assertion.params.right = right;

  if (!eq_func) {
    result.data.eq_assertion.warning = EQ_FUNC_IS_NULL_WARNING;
    result.data.eq_assertion.result = 0;
    return result;
  }

  temp = eq_func(left, right);

  // check if pair points to same address
  if (left == right) {
    result.data.eq_assertion.warning = SAME_ADDRESS_PARAMS_WARNING;

    // if same address pair returns false (not expected)
    if (!temp)
      result.data.eq_assertion.warning = SAME_ADDRESS_PARAMS_ARE_DIFF_WARNING;

    result.data.eq_assertion.result = temp;

  } else {
    result.data.eq_assertion.warning = NO_WARNING;
    result.data.eq_assertion.result = temp;
  }

  return result;
}
