#include "cli_app.h"

#define ERROR_CODE 1
#define SUCCESS_CODE 0

typedef struct cli_app app_t;

int main(int argc, const char *argv[]) {
  static app_t app = {0};
  int exit_code = SUCCESS_CODE;

  if (!app_parse(&app, --argc, ++argv))
    exit_code = ERROR_CODE;

  else if (!app_run(&app))
    exit_code = ERROR_CODE;

  return exit_code;
}
