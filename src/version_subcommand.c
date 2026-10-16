#include "version_subcommand.h"
#include <stdio.h>

void run_version_subcommand(bool is_verbose) {
  printf(
    PROGRAM_NAME " " PROGRAM_TAG " "
    "(" PROGRAM_SHORT_COMMIT_HASH " " PROGRAM_COMMIT_DATE ")"
    "\n"
  );

  if (!is_verbose)
    return;

  printf(
    "commit-hash: "    PROGRAM_LONG_COMMIT_HASH "\n"
    "commit-date: "    PROGRAM_COMMIT_DATE      "\n"
    "release: "        PROGRAM_TAG              "\n"
    "target-machine: " PROGRAM_TARGET_MACHINE   "\n"
    "compiled on: "    PROGRAM_COMPILER         "\n"
  );
}
