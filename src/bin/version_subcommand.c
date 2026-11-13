#include "version_subcommand.h"

#include <stdio.h>

void simple_printing(char *program_name);

void run_version_subcommand(char *program_name, bool verbose) {
  simple_printing(program_name);

  if (!verbose)
    return;

  printf("binary: %s\n", program_name);

  printf("commit hash: "  PROGRAM_FULL_COMMIT_HASH "\n");
  printf("commit date: "  PROGRAM_COMMIT_DATE      "\n");
  printf("host machine: " PROGRAM_HOST_MACHINE     "\n");
  printf("release tag: "  PROGRAM_TAG              "\n");

  printf("compiled with: %s %s\n", PROGRAM_COMPILER, COMPILER_VERSION);
}

void simple_printing(char *program_name) {
  printf("%s ", program_name);
  printf(PROGRAM_TAG " (" PROGRAM_SHORT_COMMIT_HASH " " PROGRAM_COMMIT_DATE ")\n");
}
