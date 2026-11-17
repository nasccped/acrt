#include "parser.h"
#include "runner.h"

int main(int argc, const char **argv) {
  static cli_t cli = { 0 };

  if (!cli_parse(&cli, argc, argv))
    return 1;

  return run(&cli) ? 0 : 1;
}
