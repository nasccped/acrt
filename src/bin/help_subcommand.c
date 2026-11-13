#include "help_subcommand.h"
#include "cli.h"
#include "cli_flags.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef MAX
#define MAX(L, R) ((L) > (R)) ? (L) : (R)
#define MINIMUM_GAP 5

void run_help_subcommand(char *program_name, bool verbose) {
  static size_t flag_gap = 0;

  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++)
    flag_gap = MAX(flag_gap, strlen(GLOBAL_FLAGS[i].long_name));

  flag_gap += MINIMUM_GAP;

  printf("%s.\n\n", PROGRAM_DESCRIPTION);

  printf("Usage: %s [FLAGS] <FILE(S)>\n\n", program_name);
  printf("Flags are:\n");

  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {
    printf("  ");

    if (GLOBAL_FLAGS[i].short_name)
      printf("-%c,", GLOBAL_FLAGS[i].short_name);
    else
      printf("   ");

    printf(" --%-*s%s\n", (int)flag_gap, GLOBAL_FLAGS[i].long_name, GLOBAL_FLAGS[i].string_description);
  }

  if (!verbose)
    return;

  // TODO: add verbose info when help asked.
}
