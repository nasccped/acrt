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
