#ifndef _ACRT_CLI_APP_H_
#define _ACRT_CLI_APP_H_

/* Refers to the app available fields. They're set within 'app_parse' function implementation (at
 * cli_app.c). */
struct cli_app {

  /* Global options used at runtime. */
  struct {

    /* verbosity mode. Note that these two options are the inverse of each other. Using them in a
     * row can leads to conflict behavior (treated at parsing). */
    int verbose, quiet;

  } global_options;

  /* Kind of subcommand being ran. */
  enum { NO_SPECIFIED, RUN_SUBCOMMAND } subcommand_kind;

  /* Subcommand required data. */
  union {
    struct { const char *path; } run;
  } subcommand;
};

/* Takes the user input (argc and argv) to parse it to the cli data structure and put all that data
 * into a cli_app struct pointer. This function returns an integer where:
 * - '0' means parsing fail;
 * - <ANY OTHER INTEGER> means parsing success.
 *
 * Since this function doesn't store parsing fail data, all the fail report must be done at this
 * function inner scope.
 *
 * Also, this function expects to get only the cli related args, so it must receives the argc
 * decremented by 1 and argv offset to next string pointer (excluding the executable path). */
int app_parse(struct cli_app *app, int argc, const char *argv[]);

/* Takes the cli app struct and runs it based on it's inner fields. Note that the app struct must
 * be data-populated before actually run (done with 'app_parse' function). This function returns an
 * integer where:
 * - '0' means run fail;
 * - <ANY OTHER INTEGER> means run success.
 *
 * Just like the 'app_parse' function, this one doesn't stores the run fail status, so it must
 * report the error within it's own scope. */
int app_run(struct cli_app *app);

#endif
