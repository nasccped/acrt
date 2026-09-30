#ifndef _ACRT_CLI_H_
#define _ACRT_CLI_H_

#include <stdint.h>

/* Max args that acrt can handle. Ensure cli paths capacity at compile time. */
#define CLI_MAX_ARGS 16

/* Acrt is a very simple cli program. It only needs a flag bitmapper (check which flags were called
 * by using bit moving operations) and a path array + it's counter to check tests cases. */
struct cli {
  uint8_t flags, path_count;
  char *paths[CLI_MAX_ARGS];
};

int cli_parse(struct cli *cli, int argc, char *argv[]);
int cli_run(struct cli *cli);

#endif
