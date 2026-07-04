#include <stdlib.h>

#include "acrt_result.h"

typedef struct __acrt_result result_t;

// Context body wrapper.
#define CTX(NAME, LINE) {.name = (NAME), .line = (LINE)}

// (RWD: result with data) creates result with data field.
#define RWD(CTX, KIND, DATA, PASSED)                                           \
  (result_t) { .context = CTX, .kind = (KIND), DATA, .passed = (PASSED) }

// (RWND: result with no data) creates result without data field.
#define RWND(CTX, KIND, PASSED)                                                \
  (result_t) { .context = CTX, .kind = (KIND), .passed = (PASSED) }

result_t acrt_result_from_int(const char *name, const unsigned int line,
                              int value) {
  return RWND(CTX(name, line), SIMPLE_BOOLEAN_ASSERTION_KIND, value != 0);
}

result_t acrt_result_from_single_pointer(const char *name,
                                         const unsigned int line,
                                         void *pointer) {
  return RWD(CTX(name, line), POINTER_BOOLEAN_ASSERTION_KIND,
             .data.single_pointer = pointer, pointer != NULL);
}
