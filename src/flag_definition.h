#ifndef _ACRT_FLAG_DEFINITION_H_
#define _ACRT_FLAG_DEFINITION_H_

#include <stdint.h>
#include <stdlib.h>

/* Flag code identifiers. */
#define _FLAG_CODE_INIT   0b1
#define FLAG_CODE_HELP    (_FLAG_CODE_INIT << 0)
#define FLAG_CODE_VERSION (_FLAG_CODE_INIT << 1)
#define FLAG_CODE_DRY_RUN (_FLAG_CODE_INIT << 2)
#define FLAG_CODE_VERBOSE (_FLAG_CODE_INIT << 3)

/* Flag definitions type (helps when parsing). */
struct flag_definition {
  char *long_name, short_name, *description;
  uint8_t code;
};

/* Flag definitions data. */
static const struct flag_definition FLAG_DEFINITIONS[] = {
  /* flag long name     flag short name     flag description                                           flag code        */
  { "help",             'h',                "Displays the help panel",                                 FLAG_CODE_HELP    },
  { "version",          'v',                "Displays the program version",                            FLAG_CODE_VERSION },
  { "dry-run",          'n',                "Simulates execution without actually running test cases", FLAG_CODE_DRY_RUN },
  { "verbose",           0 ,                "Applies verbose switch to the executed action",           FLAG_CODE_VERBOSE }
};

/* Keep track of how many flags were disposed. */
static const size_t FLAG_COUNT = sizeof(FLAG_DEFINITIONS) / sizeof(struct flag_definition);

#endif
