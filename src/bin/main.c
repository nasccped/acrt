#include "cli.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  static cli_t cli = { 0 };

  // if parsing fails.
  if (!parse_cli(&cli, argc, argv))
    return 1;

  return run_app(&cli) ? 0 : 1;
}
