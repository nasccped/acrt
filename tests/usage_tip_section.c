/* The code bellow tests the code sample at '../USAGE.md#usage-tip'. */
#include "../src/acrt.h"

acrt_t my_acrt();

int main() {
  acrt_t acrt = my_acrt();
  acrt_bool(acrt, 1);
  // expecting fail
  acrt_bool(acrt, NULL);
  // continuing assertion
  acrt_bool(acrt, "some string");
  return 0;
}

acrt_t my_acrt() {
  acrt_t acrt = acrt_new();
  acrt_set_name(&acrt, "my custom name");
  acrt_set_display_variant(&acrt, DISPLAY_ALL_ASSERTIONS);
  acrt_set_on_fail(&acrt, CONTINUE_ASSERTIONS);
  return acrt;
}
