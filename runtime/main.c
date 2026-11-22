#include "parser.h"
#include "runner.h"

#define ACRT_EXIT_SUCCESS 0
#define ACRT_EXIT_FAILURE 1

int main(int argc, const char **argv) {
  // Return if parsing and run succeeds:
  return (cli_parse(argc, argv) && run())
    ? ACRT_EXIT_SUCCESS
    : ACRT_EXIT_FAILURE;
}
