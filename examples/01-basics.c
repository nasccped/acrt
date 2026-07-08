#include "../src/acrt.h"

static int universal_true = 1;
static double pi = 3.14;
static char *text = "my string";
static void *integer_ptr = &universal_true;

int main(int argc, char *argv[]) {
  // Macro as default constructor.
  acrt_t acrt = ACRT_NEW();

  // Assert any number like value.
  ACRT_BOOL(&acrt, universal_true);
  ACRT_BOOL(&acrt, pi);
  // also literals
  ACRT_BOOL(&acrt, 2);

  // Assert pointers.
  ACRT_BOOL(&acrt, text);
  ACRT_BOOL(&acrt, integer_ptr);

  // Assert dynamic values and use it as condition.
  char *may_fail = argc > 1 ? argv[1] : NULL;

  // BUG: This only succeeds when passing args to exe call.
  if (ACRT_BOOL(&acrt, may_fail)) {
    printf("'may fail' variable holds the '%s' string.\n\n", may_fail);
  }

  printf("All assertions \e[92mdone\e[0m with no errors...\n");

  return 0;
}
