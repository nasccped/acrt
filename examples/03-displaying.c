#include "../src/acrt.h"

static acrt_t acrt = ACRT_DEFAULT;

int main() {
  acrt_set_on_fail_continue_assertions(&acrt);

  // No info is displayed (even when assertion fails)
  acrt_set_display_mode_to_quiet(&acrt);
  ACRT_BOOL(&acrt, 0);

  printf("Previous assertion was failed but not displayed!\n\n");

  // Display all info (even when assertion succeeds)
  acrt_set_display_mode_to_all(&acrt);
  ACRT_BOOL(&acrt, 10);

  printf("\nAssertion table result can also be displayed:\n");

  // Display counting table.
  acrt_display_counting(&acrt, stdout);

  // NOTE: default mode is 'DISPLAY_MODE_FAILED_ONLY'. It can be set by using
  //       the 'acrt_set_display_mode_to_failed_only'.
  return 0;
}
