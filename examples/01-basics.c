#include "../src/acrt.h"
#include <stdio.h>

int main(void) {
  // works but not recommended
  acrt_t acrt = {0};
  // use default initializer instead
  acrt = acrt_new();

  // works:
  acrt_bool(acrt, 1);
  // compile error:
  // acrt_bool(&acrt, 1);

  // compiles but crashes the program.
  // free(&acrt);

  printf("No free is required. All the objects are in the stack.\n");
  return 0;
}
