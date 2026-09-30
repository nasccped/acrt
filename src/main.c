#include "cli.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef struct cli cli_t;

int main(int argc, char *argv[]) {
  cli_t cli = { 0 };

  if (!cli_parse(&cli, argc, argv))
    return EXIT_FAILURE;

  return cli_run(&cli) ? EXIT_SUCCESS : EXIT_FAILURE;
}
