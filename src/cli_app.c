#include "cli_app.h"
#include "macro_utils.h"

#include <stdio.h>
#include <stdlib.h>

#define __POSITIVE 1
#define __NEGATIVE 0

#define PARSING_SUCCESS __POSITIVE
#define PARSING_FAIL    __NEGATIVE

#define RUN_SUCCESS __POSITIVE
#define RUN_FAIL    __NEGATIVE

typedef struct cli_app app_t;

int app_parse(app_t *app, int argc, const char *argv[]) {
  NON_NULL_OR_UNREACHABLE(app);

  if (!argc) {
    error_tag_println(
      "acrt program expects at least %sone%s argument!",
      BOLD_ESCAPE,
      RESET_ESCAPE
    );
    return PARSING_FAIL;
  }

  app->action_data.arg_count = argc;
  app->action_data.args = argv;

  return PARSING_SUCCESS;
}

int app_run(app_t *app) {
  int argc;
  const char **args;

  NON_NULL_OR_UNREACHABLE(app);

  argc = app->action_data.arg_count;
  args = app->action_data.args;

  printf("Ready to run: ");

  for (int i = 0; i < argc; i++)
    printf("%s%s", args[i], (i == (argc - 1)) ? "\n" : ", ");

  return RUN_SUCCESS;
}
