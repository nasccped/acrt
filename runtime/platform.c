#include "helpers.h"
#include "platform.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Type shorthand for input_t array + counting data group. */
typedef struct input_sequence {

  // How many inputs must be ran.
  int count;

  // Input sequence.
  input_t inputs[ACRT_BUFFER_MAX_SIZE];

} inp_seq_t;

/* Returns the input instance pointer from a input sequence struct by matching the provided name.
 * Returns NULL if not found! Note that this function expects the 'const char *' type, which means
 * that the file path must be untouched (the same as from stdin). */
static inp_inst_t *find_inp_inst_by_file_path(inp_seq_t *restrict, const char *);

/* De-initializes an input instance. Note that input instances can also hold plat_st which is
 * implemented at platform_<plat> header/sources. In other words, this function also calls the
 * plat_deinit to ensure safe-deinitializing. Later, just zero the struct fields. */
static void inp_inst_deinit(inp_inst_t *restrict);

/* Initializes an input instance by taking the original file path (taken from stdin) as
 * argument.
 *
 * This function also calls the 'plat_init' function since the input instance requires platform
 * dependent fields.
 *
 * Consider reading the 'inp_seq_init' doc to get info about error handling. */
static bool inp_inst_init(inp_inst_t *restrict, const char *);

/* Returns if the input instance path matches the provided string. There's two kind of string for a
 * file input instance:
 * - 'untouched' meaning that the string points to the same address as passed by main's function
 *   argc;
 * - 'prefixed' meaning that the string was allocated to fit the 'CURDIR_PREFIX' (defined at
 *   helpers.h).
 *
 * Anyway, the passed string is expected to be the one that taken from main's argv values (const
 * char *). This function does the conversion/offsetting by himself. */
static inline bool inp_inst_path_matches(inp_inst_t *, const char *);

/* De-initializes each input_t from the inputs array. Note that input_t can refer to:
 * - input instance concrete: normally de-inits by using the 'inp_inst_deinit' function;
 * - input instance reference: doesn't require deinit since a reference points to a concrete
 *   instance at a previous index (already de-initialized).
 *
 * Then, the input_t (and the sequence as well) inner fields are zeroed. */
static void inp_seq_deinit(inp_seq_t *restrict);

/* Initializes the input sequence by taking the path count, the path pointer (array) and the flag
 * values.
 *
 * This function not only initializes the sequence but it also initializes it's dependencies, such
 * as (in order):
 *
 * -> initialize sequence:
 *    -> initialize input instance:
 *       -> initialize platform struct.
 *
 * Each one of these functions returns a boolean meaning the operation success. The error is
 * reported directly by the function where it occurred.
 *
 * If some fail occurs on a single instance initializing, this one won't be added to the sequence,
 * which means that the instance must be de-initialized right after initialization fail. */
static bool inp_seq_init(inp_seq_t *restrict, int, const char **, size_t);

bool platform_run(int path_count, const char **paths, size_t flags) {
  static inp_seq_t is = { 0 };

  // If initialize fails:
  if (!inp_seq_init(&is, path_count, paths, flags)) {
    inp_seq_deinit(&is);
    return false;
  }

  inp_seq_deinit(&is);
  return true;
}

static inp_inst_t *find_inp_inst_by_file_path(inp_seq_t *restrict is, const char *file_path) {
  static inp_inst_t *current;
  static char *inst_fp;

  // For each already initialized instance:
  for (int i = 0; i < is->count; i++) {

    current = (is->inputs[i].kind == IK_REFERENCE)
      ? is->inputs[i].refr
      : &is->inputs[i].inst;

    inst_fp = (current->fp_kind == FPK_UNTOUCHED)
      ? (char *)current->path_untouched
      : current->path_prefixed + strlen(CURDIR_PREFIX);

    if (strcmp(inst_fp, file_path) == 0)
      return current;
  }

  // No instance found:
  return NULL;
}

static void inp_inst_deinit(inp_inst_t *restrict ii) {
  // If file path memory was allocated:
  if (ii->fp_kind == FPK_PREFIXED && ii->path_prefixed)
    free(ii->path_prefixed);

  plat_deinit(&ii->plat);
  *ii = (inp_inst_t){ 0 };
}

static bool inp_inst_init(inp_inst_t *restrict ii, const char *path) {
  static char *file_path;

  // If file path requires curdir prefix:
  if (path_requires_curdir_prefix((char *)path)) {
    ii->fp_kind = FPK_PREFIXED;
    file_path = (char *)malloc(sizeof(char) * (strlen(path) + strlen(CURDIR_PREFIX) + 1));

    // If allocation fails:
    if (!file_path) {
      TAGERR("failed to allocate '%s' requirements.", path);
      return false;
    }

    // Early set so it will be freed at 'input_instance_deinit' if 'sprintf' fails.
    ii->path_prefixed = file_path;

    // If string formatting fails:
    if (sprintf(file_path, "%s%s", CURDIR_PREFIX, path) < 0) {
      TAGERR("failed to format input file path => sprintf(\"%%s%%s\", \"%s\", \"%s\")",
             CURDIR_PREFIX, path);
      return false;
    }
  }

  // No curdir prefix required:
  else {
    ii->fp_kind = FPK_UNTOUCHED;
    ii->path_untouched = path;

    // Set on file_path to be latter used.
    file_path = (char *)path;
  }

  // Platform specific initializing.
  return plat_init(&ii->plat, file_path);
}

static inline bool inp_inst_path_matches(inp_inst_t *ii, const char *path) {
  static char *p1;

  p1 = (ii->fp_kind == FPK_UNTOUCHED)
    ? (char *)(ii->path_untouched)
    : (ii->path_prefixed + strlen(CURDIR_PREFIX));

  return strcmp(p1, (char *)path) == 0;
}

static void inp_seq_deinit(inp_seq_t *restrict is) {
  // For each input on input sequence:
  for (int i = 0; i < is->count; i++) {

    // If input kind refers to concrete instance:
    if (is->inputs[i].kind == IK_INSTANCE)
      inp_inst_deinit(&is->inputs[i].inst);

    is->inputs[i] = (input_t){ 0 };
  }

  is->count = 0;
}

static bool inp_seq_init(inp_seq_t *restrict is, int count, const char **paths, size_t flags) {
  static input_t input;
  static inp_inst_t instance, *pointer;

  // For each path:
  for (int i = 0; i < count; i++) {

    // If file instance with the provided file path already exists:
    if ((pointer = find_inp_inst_by_file_path(is, paths[i]))) {

      // '--rerun' flag not being used:
      if (!FLAG_INTERSECTION(flags, FCI_RERUN)) {
        TAGERR("input duplicates were detected ('%s').", paths[i]);
        PRINTERR("Duplicates aren't allowed by default. This prevents static values\n"
                 "mess up and unexpected behavior, but it can still be forced by\n"
                 "using the '--%s' flag.\n\n", FLN_RERUN);
        TAGTIP(stderr,
               "the '--%s' flag doesn't create two file input instances. Actually,\n"
               "it creates a single instance and runs it twice (or more times) through\n"
               "a reference.",
               FLN_RERUN);

        return false;
      }

      input = (input_t){ .kind = IK_REFERENCE, .refr = pointer };
    }

    // Else (no instance for this file path)
    else {

      // If instance init fails:
      if (!inp_inst_init(&instance, paths[i])) {
        inp_inst_deinit(&instance);
        return false;
      }

      input = (input_t){ .kind = IK_INSTANCE, .inst = instance };
    }

    /* NOTE: buffer size checking isn't required since ACRT_BUFFER_MAX_SIZE was already checked at
     * runner.c scope. */
    is->inputs[i] = input;
    is->count++;
  }

  return true;
}
