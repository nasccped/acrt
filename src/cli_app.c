#include "cli_app.h"
#include "macro_utils.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __POSITIVE 1
#define __NEGATIVE 0

#define PARSING_SUCCESS __POSITIVE
#define PARSING_FAIL    __NEGATIVE

#define RUN_SUCCESS __POSITIVE
#define RUN_FAIL    __NEGATIVE

typedef struct cli_app app_t;

/* Clears the app inner fields. This function is local since app cleaning is only required after
 * parsing/running. */
static void app_clear(app_t *app);

int app_parse(app_t *app, int argc, const char *argv[]) {
  NON_NULL_OR_UNREACHABLE(app);

  // check for any empty string.
  for (int i = 0; i < argc; i++) {
    if (string_is_whitespace(argv[i])) {
      error_tag_println("empty strings aren't allowed as arguments!");
      goto fail;
    }
  }

  // no specified subcommand as default
  app->subcommand_kind = NO_SPECIFIED;

  // for each arg.
  for (; argc; argv++, argc--) {
    if (string_equals("-q", argv[0]) || string_equals("--quiet", argv[0])) {
      if (app->global_options.verbose) {
        error_tag_println(
          "%squiet%s option conflicts with %sverbose%s.",
          BOLD_ESCAPE,
          RESET_ESCAPE,
          BOLD_ESCAPE,
          RESET_ESCAPE
        );
        eprintln("They must be used separately!");
        goto fail;
      }

      app->global_options.quiet++;
      continue;
    }

    if (string_equals("--verbose", argv[0])) {
      if (app->global_options.quiet) {
        error_tag_println(
          "%sverbose%s option conflicts with %squiet%s.",
          BOLD_ESCAPE,
          RESET_ESCAPE,
          BOLD_ESCAPE,
          RESET_ESCAPE
        );
        eprintln("They must be used separately!");
        goto fail;
      }

      app->global_options.verbose++;
      continue;
    }

    if (string_equals("run", argv[0])) {
      if (!(--argc)) {
        error_tag_println(
          "%srun%s command requires a %spath%s.",
          BOLD_ESCAPE,
          RESET_ESCAPE,
          ITALIC_ESCAPE,
          RESET_ESCAPE
        );
        eprintln(
          "Consider using %shelp%s to get a more detailed overview!",
          GREEN_ESCAPE,
          RESET_ESCAPE
        );
        goto fail;
      }

      app->subcommand_kind = RUN_SUBCOMMAND;
      argv++;
      app->subcommand.run.path = argv[0];
      continue;
    }
  }

  // HACK: the line bellow always refers to parsing success and it will ever be reached BEFORE
  // parsing fail return. The parsing fail value will only be returned when calling goto within
  // a fail-case branch state.
  return PARSING_SUCCESS;

fail:
  app_clear(app);
  return PARSING_FAIL;
}

int app_run(app_t *app) {
  NON_NULL_OR_UNREACHABLE(app);

  switch (app->subcommand_kind) {
    case NO_SPECIFIED:
      break;

    case RUN_SUBCOMMAND:
      println("Ready to run %s'%s'%s test cases!", ITALIC_ESCAPE, app->subcommand.run.path, RESET_ESCAPE);
      break;
  }

  app_clear(app);
  return RUN_SUCCESS;
}

static void app_clear(app_t *app) {
  NON_NULL_OR_UNREACHABLE(app);

  switch (app->subcommand_kind) {
    // when cleaning not required
    case NO_SPECIFIED:
      break;

    case RUN_SUBCOMMAND:
      break;
  }
}
