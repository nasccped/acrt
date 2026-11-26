#include "cli.h"
#include "helpers.h"
#include "pformat.h"

/* Platform including. ------------------------------------------------------------------------- */
#if defined(__linux__) || defined(__unix__)
# include "platform_unix.h"
#else
# error "Missing implementation for the current machine. See 'Platform including' line at runner.c"
#endif
/* --------------------------------------------------------------------------------------------- */

#include "runner.h"

#include <string.h>

// Tip shorthand when flag expects only verbose flag.
#define FLAG_CAN_ONLY_BE_USED_WITH_VERBOSE_ERR(FLAG_NAME) do { \
  TAGERR("'--%s' can only be used with '--%s'/'-%c' flag.",    \
         (FLAG_NAME), FLAG_VERBOSE.name, FLAG_VERBOSE.alias);  \
} while (0)

#ifdef MAX
# undef MAX
#endif

#define MAX(L, R) ((L) > (R)) ? (L) : (R)
#define MINIMUM_SPRINTF_SUCCESS 0

/* Data structure helper to map flag subcommands (such as '--help' or '--version') to actual
 * functions. Note that the function pointer must return a bool since is't required by the
 * 'run' API. Logical checks are done at function's scope + error is report on it as well. */
struct flag_runner_map {

  const struct flag *flag;

  bool (*func)();
};

/* Data structure referring to a input name. Input names can refers to these variants:
 * - prefixed: when the input name requires CURDIR_PREFIX_STR to be successfully handled by the
 *   API's function;
 * - unprefixed: when the input name doesn't required CURDIR_PREFIX (already includes prefix or
 *   refers to an absolute path).
 *
 * These variants and values are handled on a 'tagged-enum' like behavior. Note that the unprefixed
 * value points to a constant char pointer since this untouched string is taken from argv (which is
 * also constant). */
struct input_name {

  enum { NK_PREFIXED, NK_UNPREFIXED, } name_kind;

  union {
    char *pref_name;
    const char *unpref_name;
  };
};

/* Definition for a concrete input instance. This data type holds all inspection required fields +
 * a platform dependent struct definition. */
struct input_instance {

  // Input naming that refers to the current input instance.
  struct input_name name;

  // Platform dependent struct defined at platform_<PLAT>.h file.
  struct platform_dependent pd;
};

/* Available variants for input sequence. Keep in mind that a file which is mentioned two or more
 * times must point to a single instance, and then, this SAME INSTANCE must be ran. Multiple
 * instances from the same file can't be created (according to 'dlopen' documentation and use cases
 * concept).
 *
 * This struct also works as 'tagged-enum', where can points either for a input instance reference
 * or hold it's own concrete instance. */
struct input_variant {

  enum { VK_REFERENCE, VK_INSTANCE } variant_kind;

  union {
    struct input_instance *refr,
    inst;
  };
};

// Refers to a sequence of inputs. Just an alternative for indexing and array handling.
struct input_sequence {

  struct input_variant inputs[ACRT_BUFFER_MAX_SIZE];

  int count;
};

typedef struct cli cli_t;
typedef struct flag flag_t;
typedef struct flag_runner_map frm_t;
typedef struct input_instance inpinst_t;
typedef struct input_name inpname_t;
typedef struct input_sequence inpseq_t;
typedef struct input_variant inpvar_t;

/* De-initializes an input instance's inner fields. Since some fields are platform-impl restricted,
 * this function must also call the 'platdep_deinit'. */
static void inpinst_deinit(inpinst_t *);

/* Initializes an input instance's inner fields. This function also takes a const char argument
 * which is used to initialize the input name field properly + be passed to platform dependent
 * initializers.
 *
 * If initialization fails, the error is reported but the current instance pointer
 * isn't de-initialized by default. Instead, the check must be done by using the return value,
 * something like:
 * ```
 * if (!inpinst_init(pointer, path_str)) {
 *   inpinst_init(pointer);
 *   return false;
 * }
 * ``` */
static bool inpinst_init(inpinst_t *, const char *);

// Returns the input name string based on it's inner fields ((un)prefixed).
static inline char *inpname_get_name(inpname_t *restrict);

/* Does the same as 'inpname_get_name' but it returns the input path as passed (without prefix, if
 * some). */
static inline char *inpname_get_unprefixed_name(inpname_t *restrict);

/* Returns if the input path string requires the curdir prefix. */
static inline bool input_path_requires_prefix(const char *);

/* Clears the input sequence inner field. It calls the platform dependent struct de-initializers +
 * input instance free's (when necessary). */
static void inpseq_clear();

/* Returns an input instance pointer from the input sequence by matching the provided path (or
 * NULL if not found). */
static inpinst_t *inpseq_find_instance_by_path(const char *);

/* Populates the input sequence based on global_cli fields (after parse). It returns a boolean
 * meaning the operation success. If something goes, it reports the error to stdout.
 *
 * WARN: it doesn't de-initializes the input sequence. It must be clear with inpseq_clear at
 * caller's function body. */
static bool inpseq_populate();

/* Runs the '--dump-version' kind of flag. This function also does other checks at cli fields
 * looking for conflicting state:
 * - it'll report the error to stderr and return false if so;
 * - otherwise, prints the program version (same as tag annotation) to stdout and returns true. */
static bool run_dump_version();

/* Normal execution of acrt program (file entries). */
static inline bool run_inputs();

/* Runs the '--help' kind of flag. All the extra checks are done the same as 'run_dump_version'
 * function. */
static bool run_help();

/* Runs the '--version' kind of flag. All the extra checks are done the same as 'run_dump_version'
 * function. */
static bool run_version();

extern const flag_t FLAG_DUMP_VERSION;
extern const flag_t FLAG_HELP;
extern const flag_t FLAG_MAY_EMPTY;
extern const flag_t FLAG_RERUN;
extern const flag_t FLAG_SUPPRESS_WARNINGS;
extern const flag_t FLAG_VERBOSE;
extern const flag_t FLAG_VERSION;
extern const flag_t *GLOBAL_FLAGS[];
extern const size_t GLOBAL_FLAGS_COUNT;

// Global cli defined/initialized at main.c
extern cli_t global_cli;

/* Mapping of flags -> runner functions. Note that this function runner searching is done on linear
 * iteration. It means that top instance have greater priorities! */
static const frm_t FLAG_RUNNER_MAPPINGS[] = {
  // flag code identifier   function pointer
  { &FLAG_HELP,              run_help         },
  { &FLAG_DUMP_VERSION,      run_dump_version },
  { &FLAG_VERSION,           run_version      },
};

// Counting of RUNNER_MAPPINGS (helps on linear searching).
static const size_t FLAG_RUNNER_MAPPING_COUNT = sizeof(FLAG_RUNNER_MAPPINGS) / sizeof(frm_t);

static inpseq_t inpseq = { 0 };

static void inpinst_deinit(inpinst_t *inpinst) {
  if (inpinst->name.name_kind == NK_PREFIXED)
    free(inpinst->name.pref_name);

  platdep_deinit(&inpinst->pd);
  *inpinst = (inpinst_t){ 0 };
}

static bool inpinst_init(inpinst_t *inpinst, const char *path) {
  static inpname_t name;
  static char *name_str;

  if (input_path_requires_prefix(path)) {
    name_str = (char *)malloc(
      sizeof(char) * (strlen(path) + strlen(CURDIR_PREFIX_STR) + NULL_TERM_LEN)
    );

    if (!name_str) {
      TAGERR("failed to allocate input instance resources (read [%s:%d]).", __FILE__, __LINE__);
      return false;
    }

    else if (_sprintf(name_str, "%s%s", CURDIR_PREFIX_STR, path) < MINIMUM_SPRINTF_SUCCESS) {
      TAGERR("failed to write input instance resources (read [%s:%d]).", __FILE__, __LINE__);
      free(name_str);
      return false;
    }

    name.name_kind = NK_PREFIXED;
    name.pref_name = name_str;
  }

  else {
    name.name_kind = NK_UNPREFIXED;
    name.unpref_name = path;
  }

  inpinst->name = name;

  if (!platdep_init(&inpinst->pd, inpname_get_name(&inpinst->name)))
    return false;

  return true;
}

static inline char *inpname_get_name(inpname_t *restrict inpname) {
  return inpname->name_kind == NK_PREFIXED
    ? inpname->pref_name
    : (char *)inpname->unpref_name;
}

static inline char *inpname_get_unprefixed_name(inpname_t *restrict inpname) {
  return inpname->name_kind == NK_PREFIXED
    ? inpname->pref_name + strlen(CURDIR_PREFIX_STR)
    : (char *)inpname->unpref_name;
}

static void inpseq_clear() {
  static inpvar_t *cur_variant;

  for (int i = 0; i < inpseq.count; i++) {
    cur_variant = &inpseq.inputs[i];

    if (cur_variant->variant_kind == VK_INSTANCE)
      inpinst_deinit(&(inpseq.inputs[i].inst));

    inpseq.inputs[i] = (inpvar_t){ 0 };
  }

  inpseq.count = 0;
}

static inpinst_t *inpseq_find_instance_by_path(const char *path) {
  static inpinst_t *ref;

  for (int i = 0; i < inpseq.count; i++) {
    ref = inpseq.inputs[i].variant_kind == VK_INSTANCE
      ? &inpseq.inputs[i].inst
      : inpseq.inputs[i].refr;

    if (strcmp(path, inpname_get_unprefixed_name(&ref->name)) == 0)
      return ref;
  }

  return NULL;
}

static bool inpseq_populate() {
  static const char *current_path;
  static inpinst_t inp_inst, *inp_ptr;
  static inpvar_t inp_var;

  for (int i = 0; i < global_cli.path_count; i++) {
    current_path = global_cli.paths[i];

    if ((inp_ptr = inpseq_find_instance_by_path(current_path))) {
      if (!flag_code_matches(&FLAG_RERUN, global_cli.flags)) {
        TAGERR("input path duplicates (%s).", current_path);
        PRINTERR("\nBy default, a file path isn't allowed to be passed two or more times!\n"
                 "This is intended to prevent static variables messing up, but can be\n"
                 "forced by using the '--%s' flag.\n", FLAG_RERUN.name);

        if (flag_code_matches(&FLAG_VERBOSE, global_cli.flags)) {
          PRINTERR("\n");
          TAGTIP(_stderr, "passing the same file won't load two instances of an object!\n"
                         "Actually, it'll load a single instance, and then, point to the\n"
                         "same instance of that file (according to program's use case).");
        }

        return false;
      }

      inp_var = (inpvar_t){ .variant_kind = VK_REFERENCE, .refr = inp_ptr };
    }

    else {
      if (!inpinst_init(&inp_inst, current_path)) {
        inpinst_deinit(&inp_inst);
        return false;
      }

      inp_var = (inpvar_t){ .variant_kind = VK_INSTANCE, .inst = inp_inst };
    }

    inpseq.inputs[inpseq.count++] = inp_var;
  }

  return true;
}

static inline bool input_path_requires_prefix(const char *path) {
  return (
    strncmp(path, CURDIR_PREFIX_STR, strlen(CURDIR_PREFIX_STR)) != 0
    && strncmp(path, ROOTDIR_PREFIX_STR, strlen(ROOTDIR_PREFIX_STR)) != 0
  );
}

bool run() {
  for (size_t i = 0; i < FLAG_RUNNER_MAPPING_COUNT; i++) {
    if (flag_code_matches(FLAG_RUNNER_MAPPINGS[i].flag, global_cli.flags)) {
      global_cli.flags = flag_code_remove(global_cli.flags, FLAG_RUNNER_MAPPINGS[i].flag->code);
      return FLAG_RUNNER_MAPPINGS[i].func();
    }
  }

  // If no flag matches, do normal execution:
  return run_inputs();
}

static bool run_dump_version() {
  // If any other argument than dump-version flag:
  if (global_cli.flags || global_cli.path_count) {
    TAGERR("'--%s' flag can't be used with other flags or path arguments.", FLAG_DUMP_VERSION.name);
    return false;
  }

  _printf("%s\n", PROGRAM_TAG);
  return true;
}

static bool run_help() {
  // Flag description gap (fdg) between long name and it's description string.
  static size_t fdg = 0;

  // If any other value than verbose:
  if (flag_code_remove(global_cli.flags, FLAG_VERBOSE.code) || global_cli.path_count) {
    FLAG_CAN_ONLY_BE_USED_WITH_VERBOSE_ERR(FLAG_HELP.name);
    return false;
  }

  // Get greatest flag description gap:
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++)
    fdg = MAX(fdg, strlen(GLOBAL_FLAGS[i]->name));

  // Increment on 3:
  fdg += 3;

  _printf("%s.\n\n", PROGRAM_DESCRIPTION);
  _printf("Usage: %s [OPTION(S)] [FILE_PATH(S)]\n\n", global_cli.self_path);
  _printf("Options:\n");

  // Print each flag info:
  for (size_t i = 0; i < GLOBAL_FLAGS_COUNT; i++) {
    set_left_gap(2);

    if (GLOBAL_FLAGS[i]->alias)
      _printf("-%c, ", GLOBAL_FLAGS[i]->alias);
    else
      _printf("    ");

    _printf("--%-*s %s\n",
           (int)fdg, GLOBAL_FLAGS[i]->name, GLOBAL_FLAGS[i]->description);
  }

  // If not verbose:
  if (!flag_code_matches(&FLAG_VERBOSE, global_cli.flags)) {
    _printf("\n");
    TAGTIP(_stdout, "you can use '--%s' flag to get a more descriptive info.", FLAG_VERBOSE.name);
    return true;
  }

  // TODO: add verbose info...

  return true;
}

static bool run_inputs() {
  static bool success;

  if (!global_cli.path_count) {

    if (!flag_code_matches(&FLAG_MAY_EMPTY, global_cli.flags)) {
      TAGERR("missing input file(s)/flag(s).");
      PRINTERR("\nRuns without files aren't allowed by default but can be forced\n"
               "with the '--%s' flag!\n\n", FLAG_MAY_EMPTY.name);
      TAGTIP(_stderr, "consider using '--%s' to get an usage overview!", FLAG_HELP.name);
      return false;
    }

    // only warn when no suppress warns.
    if (!flag_code_matches(&FLAG_SUPPRESS_WARNINGS, global_cli.flags)) {
      TAGWARN(_stdout, "no files were passed.");
      _printf("Exiting normally!\n");
    }

    return true;
  }

  if (!inpseq_populate()) {
    inpseq_clear();
    return false;
  }

  success = inpseq_run();
  inpseq_clear();

  return success;
}

static bool run_version() {
  // other args than verbose aren't allowed.
  if (flag_code_remove(global_cli.flags, FLAG_VERBOSE.code) || global_cli.path_count) {
    FLAG_CAN_ONLY_BE_USED_WITH_VERBOSE_ERR(FLAG_VERSION.name);
    return false;
  }

  else if (!flag_code_matches(&FLAG_VERBOSE, global_cli.flags))
    _printf("%s %s (%s %s)\n",
           global_cli.self_path,
           PROGRAM_TAG,
           PROGRAM_SHORT_COMMIT_HASH,
           PROGRAM_COMMIT_DATE
    );

  else {
   _printf("binary: %s\n", global_cli.self_path);
   _printf("commit hash: %s\n", PROGRAM_FULL_COMMIT_HASH);
   _printf("commit date: %s\n", PROGRAM_COMMIT_DATE);
   _printf("host machine: %s\n", PROGRAM_HOST_MACHINE);
   _printf("release tag: %s\n", PROGRAM_TAG);
   _printf("compiled with: %s %s\n", PROGRAM_COMPILER, COMPILER_VERSION);
  }

  return true;
}
