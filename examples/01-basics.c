#include "../src/acrt.h"

int main(int argc, char *argv[]) {
  // Macro as default constructor.
  acrt_t acrt = ACRT_NEW();

  // Assert any number like value.
  ACRT_BOOL(&acrt, 1);
  ACRT_BOOL(&acrt, (int)3.14);

  // Assert pointers.
  int some_integer = 3;
  int *int_ptr = &some_integer;
  char *char_ptr = "my string";
  ACRT_BOOL(&acrt, int_ptr);
  ACRT_BOOL(&acrt, char_ptr);

  // Assert dynamic values and use it as condition.
  char *may_fail = argc > 1 ? argv[1] : NULL;

  // BUG: This only succeeds when passing args to exe call.
  if (ACRT_BOOL(&acrt, may_fail)) {
    printf("may fail holds the '%s' string.\n\n", may_fail);
  }

  printf("All assertions \e[92mdone\e[0m with no errors...\n");

  return 0;
}
