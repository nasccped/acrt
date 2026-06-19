#include "../src/acrt.h"
#include <stdio.h>
#include <stdlib.h>

acrt_t my_acrt();

int main() {
  int *pointer;
  if (!(pointer = malloc(sizeof(int)))) {
    fprintf(stderr, "pointer malloc failed.\naborting!\n");
    return 1;
  }
  acrt_t acrt = my_acrt();
  // asserting integers
  acrt_bool(acrt, 0);   // fail
  acrt_bool(acrt, 1);   // pass
  acrt_bool(acrt, 128); // pass
  // asserting pointers
  acrt_bool(acrt, NULL);             // fail
  acrt_bool(acrt, "string literal"); // pass
  acrt_bool(acrt, pointer);          // pass
  // asserting expressions
  acrt_bool(acrt, !0);     // pass
  acrt_bool(acrt, 1 == 1); // pass

  free(pointer);
  return 0;
}

acrt_t my_acrt() {
  acrt_t acrt = acrt_new();
  acrt_set_name(&acrt, "boolean assertions");
  acrt_set_display_variant(&acrt, DISPLAY_ALL_ASSERTIONS);
  acrt_set_on_fail(&acrt, CONTINUE_ASSERTIONS);
  return acrt;
}
