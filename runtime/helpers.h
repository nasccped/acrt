#ifndef _ACRT_RUNTIME_HELPERS_H_
#define _ACRT_RUNTIME_HELPERS_H_

/* This file provides runtime definitions and macro helpers to avoid out-of-context including
 * between non related source files (turns debugging and implementing little difficulty). */

// acrt's arrays max size (avoid malloc and arena usage).
#define ACRT_BUFFER_MAX_SIZE 64

// Adds a bold ansi escape enclosing to the provided string (must be string literal).
#define BOLD_ESCAPE_ENCLOSING(STR) "\x1b[1m" STR "\x1b[0m"

// Shorthand for fprintf(stderr, ...). Doesn't add newline at the end.
#define PRINTERR(ARGS...) fprintf(stderr, ARGS)

// Prints a tagged error message to stderr + adds a newline at the end.
#define TAGERR(ARGS...) do {               \
  PRINTERR("\x1b[91merror\x1b[0m: " ARGS); \
  PRINTERR("\n");                          \
} while (0)

// Works similar to TAGERR but it takes a FILE pointer to prints to and uses the 'tip' prefix.
#define TAGTIP(FILE_TARG, ARGS...) do {              \
  fprintf((FILE_TARG), "\x1b[96mtip\x1b[0m: " ARGS); \
  fprintf((FILE_TARG), "\n");                        \
} while (0)

// Works similar to TAGTIP but uses the 'warn' prefix.
#define TAGWARN(FILE_TARG, ARGS...) do {              \
  fprintf((FILE_TARG), "\x1b[93mwarn\x1b[0m: " ARGS); \
  fprintf((FILE_TARG), "\n");                         \
} while (0)

// Refers to '\0'.
#define NULL_TERM_LEN 1

#if defined(__linux__) || defined(__unix__)
# define CURDIR_PREFIX_STR  "./"
# define ROOTDIR_PREFIX_STR "/"
#else
# error "No default definition(s) for this platform. See helpers.h!"
#endif

#endif
