#include "../src/acrt.h"
#include <string.h>

int main() {
  acrt_t acrt = acrt_new();
  // NOTE: check if no previous first.
  acrt_bool(acrt, acrt.previous == NO_PREVIOUS_ASSERTION);
  // WARN: check counting to guarantee starts with zero (before other
  // assertions)
  acrt_bool(acrt, acrt.counter.total == 1 && acrt.counter.passed == 1 &&
                      acrt.counter.failed == 0);
  acrt_bool(acrt, strcmp(acrt.name, "UNNAMED ACRT") == 0);
  acrt_bool(acrt, acrt.display_mode == DISPLAY_ERRORS_ONLY);
  acrt_bool(acrt, acrt.on_fail == EXIT_PROGRAM);
  return 0;
}
