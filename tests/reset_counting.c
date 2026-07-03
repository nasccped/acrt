#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/acrt.h"
#include "test_util.h"

typedef struct __acrt_counting counting_t;

static acrt_t acrt;
static counting_t counting;

// Does the counting assertion.
int assert_counting(counting_t expected);

// If two counting structs contains the same inner data.
int counting_are_equals(counting_t *left, counting_t *right);

// Converts the counting struct in a visual format (string).
char *counting_to_str(counting_t *self);

int main() {
  acrt = ACRT_NEW();

  acrt.__context.counting.total = 5;
  acrt.__context.counting.passed = 2;
  acrt.__context.counting.failed = 2;
  acrt.__context.counting.ignored = 1;

  counting = (counting_t){.total = 5, .passed = 2, .failed = 2, .ignored = 1};

  if (!assert_counting(counting))
    return 1;

  acrt_reset_counting(&acrt);
  counting = (counting_t){.total = 0, .passed = 0, .failed = 0, .ignored = 0};

  if (!assert_counting(counting))
    return 1;

  return 0;
}

int assert_counting(counting_t expected) {
  ASSERT_EQ_CUSTOM("CountingEq", &expected, &acrt.__context.counting,
                   counting_are_equals, counting_to_str);
  return ASSERTION_PASSED;
}

int counting_are_equals(counting_t *left, counting_t *right) {
  if (!left || !right)
    return 0;

  return left->failed == right->failed && left->ignored == right->ignored &&
         left->total == right->total && left->passed == right->passed;
}

char *counting_to_str(counting_t *self) {
  char *buffer;
  if (!(buffer = malloc(256)))
    return NULL;

  snprintf(buffer, 256,
           "Counting { .total = %d, .passed = %d, .failed = %d, .ignored = %d}",
           self->total, self->passed, self->failed, self->ignored);
  return buffer;
}
