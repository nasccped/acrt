#include "../src/acrt.h"

int main() {
  acrt_t acrt = acrt_new();
  // new acrt group name
  acrt_set_name(&acrt, "custom name");
  // all assertions gonna be displayed
  acrt_set_display_variant(&acrt, DISPLAY_ALL_ASSERTIONS);
  // continue program (even with failed assertions)
  acrt_set_on_fail(&acrt, CONTINUE_ASSERTIONS);

  acrt_bool(acrt, 1 > 0);            // success
  acrt_bool(acrt, 0.1 + 0.2 == 0.3); // fail (float precision thing).
  acrt_bool(acrt, 10 / 5 == 2);      // success again

  return 0;
}
