#include "cli.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  static cli_t cli = { 0 };

  // if parsing fails.
  if (!parse_cli(&cli, argc, argv))
    return 1;

  printf("--- Cli succefully parsed ---\n");
  printf(" Flag value: %ld\n", cli.global_flags);
  printf(" Paths:\n");

  for (int i = 0; i < cli.path_count; i++)
    printf("   [%2d] %s\n", i + 1, cli.paths[i]);

  return 0;
}
