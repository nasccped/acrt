#include "flag_definition.h"
#include "help_action.h"

#include <stdio.h>
#include <string.h>

/* Set's the origin repository via Makefile helper. */
#define ORIGIN_REPOSITORY_URL _MK_REPO_URL

/* Gap between the flag long name + it's description. */
#define FLAG_DESCRIPTION_GAP 15

#define LEFT_GAP_WHITESPACE "    "

typedef struct flag_definition flag_definition_t;

void run_help_action(int verbose) {
  static const flag_definition_t *fd;
  static int current_gap;

  puts("Test case runner for the acrt framework.\n"); // yes, use double new-line.
  puts("Usage: acrt [options] path(s)\n");
  puts("Options:");

  for (size_t i = 0; i < FLAG_COUNT; i++) {
    fd = &FLAG_DEFINITIONS[i];
    current_gap = (int)(FLAG_DESCRIPTION_GAP - strlen(fd->long_name));

    printf(LEFT_GAP_WHITESPACE);

    if (fd->short_name != 0)
      printf("-%c, ", fd->short_name);
    else
      printf("    ");

    printf("--%s%*s%s\n", fd->long_name, current_gap, "", fd->description);
  }

  if (!verbose)
    return;

  printf("\nNote: actual run requires the acrt library. You can find\n"
           "      a more detailed overview on acrt's official repository:\n"
           "      - %s\n", ORIGIN_REPOSITORY_URL);
}
