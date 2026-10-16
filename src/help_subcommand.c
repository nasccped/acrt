#include "cli.h"
#include "help_subcommand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Returns the length of the greatest flag (long name). */
static int get_greatest_flag_length();

void run_help_subcommand(const char *bin_path, bool is_verbose) {
  static const char *long_name;
  static char short_name;
  static const char *description;
  static int desc_gap;

  // search for the greatest flag + add 4 spaces gap
  desc_gap = get_greatest_flag_length() + 4;

  printf("Ergonomic test runtime for C programs.\n\n");
  printf("Usage: %s <OPTION(S)> [SHARED-OBJECT(S)...]\n\n", bin_path);
  printf("Options:\n");

  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {
    long_name = GLOBAL_FLAGS[i].long_name;
    short_name = GLOBAL_FLAGS[i].short_name;
    description = GLOBAL_FLAGS[i].description;

    printf("  ");

    if (short_name != 0)
      printf("-%c, ", short_name);

    else 
      printf("    ");

    printf("--%-*s%s\n", desc_gap, long_name, description);
  }

  // Skip 1 line (verbose or not).
  printf("\n");

  if (!is_verbose) {
    printf("Consider using --verbose flag to get a more detailed overview!\n");
    return;
  }

  // TODO: add verbose info.
}

static int get_greatest_flag_length() {
  static int length = 0, current = 0;

  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {
    current = (int)strlen(GLOBAL_FLAGS[i].long_name);
    length = (current > length) ? current : length;
  }

  return length;
}
