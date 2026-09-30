#include "version_action.h"

#include <stdio.h>

#define PROGRAM_NAME              "acrt"
#define PROGRAM_TAG               _MK_PROJ_TAG
#define PROGRAM_SHORT_COMMIT_HASH _MK_PROJ_SHORT_COMMIT_HASH
#define PROGRAM_FULL_COMMIT_HASH  _MK_PROJ_FULL_COMMIT_HASH
#define PROGRAM_COMMIT_DATE       _MK_PROJ_COMMIT_DATE
#define PROGRAM_TARGET_MACHINE    _MK_PROGRAM_TARGET_MACHINE
#define PROGRAM_COMPILER_VERSION  _MK_COMPILER_VERSION

void run_version_action(int verbose) {
  printf("%s %s (%s %s)\n",
         PROGRAM_NAME, PROGRAM_TAG, PROGRAM_SHORT_COMMIT_HASH, PROGRAM_COMMIT_DATE);

  if (!verbose)
    return;

  printf(
    "commit-hash: %s\n"
    "commit-date: %s\n"
    "release: %s\n"
    "target-machine: %s\n"
    "compiler: gcc %s\n",
    PROGRAM_FULL_COMMIT_HASH, PROGRAM_COMMIT_DATE, PROGRAM_TAG,
    PROGRAM_TARGET_MACHINE, PROGRAM_COMPILER_VERSION);
}
