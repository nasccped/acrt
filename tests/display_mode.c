#include <stdio.h>
#include <string.h>

#include "../src/acrt.h"
#include "test_util.h"

#define DISPLAY_MODE_TO_STR(MODE)                                              \
  (MODE) == DISPLAY_MODE_FAILED_ONLY ? "DISPLAY_MODE_FAILED_ONLY"              \
  : (MODE) == DISPLAY_MODE_FAILED_AND_PASSED                                   \
      ? "DISPLAY_MODE_FAILED_AND_PASSED"                                       \
  : (MODE) == DISPLAY_MODE_ALL   ? "DISPLAY_MODE_ALL"                          \
  : (MODE) == DISPLAY_MODE_QUIET ? "DISPLAY_MODE_QUIET"                        \
                                 : "UNDEFINED MODE"

static acrt_t acrt;

// Does the display mode assertion.
int assert_display_mode(acrt_display_mode_t expected);

int main() {
  acrt = ACRT_NEW();

  if (!assert_display_mode(DISPLAY_MODE_FAILED_ONLY))
    return 1;

  acrt_display_mode(&acrt, DISPLAY_MODE_ALL);

  if (!assert_display_mode(DISPLAY_MODE_ALL))
    return 1;

  acrt_display_mode(&acrt, DISPLAY_MODE_FAILED_AND_PASSED);

  if (!assert_display_mode(DISPLAY_MODE_FAILED_AND_PASSED))
    return 1;

  return 0;
}

int assert_display_mode(acrt_display_mode_t expected) {
  ASSERT_EQ_WITH_STRING_FUNC("DisplayMode", expected,
                             acrt.__context.display_mode, DISPLAY_MODE_TO_STR);
  return ASSERTION_PASSED;
}
