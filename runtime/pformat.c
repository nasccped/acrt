#include "pformat.h"
#include "cli.h"

#include <stdarg.h>
#include <stdio.h>

#define IS_QUIET            flag_code_matches(&FLAG_QUIET, global_cli.flags)
#define WARNINGS_SUPPRESSED flag_code_matches(&FLAG_SUPPRESS_WARNINGS, global_cli.flags)

typedef struct cli cli_t;
typedef enum file_identifier fident_t;
typedef struct flag flag_t;

static va_list ap;
static unsigned int left_gap = 0;
static int return_value;

extern cli_t global_cli;
extern flag_t FLAG_QUIET;
extern flag_t FLAG_SUPPRESS_WARNINGS;

// Returns the properly file pointer from a given file identifier.
static inline FILE *file_from_fident(fident_t);

// Prints the left gap whitespace to the static file pointer.
static inline int print_left_gap(FILE *restrict);

int __print_tag_warn(fident_t fi, const char *restrict format, ...) {
  static FILE *f;

  if (IS_QUIET || WARNINGS_SUPPRESSED)
    return 0;

  f = file_from_fident(fi);

  va_start(ap, format);
  return_value = fprintf(f, "\x1b[93mwarn\x1b[0m: ") + vfprintf(f, format, ap) + fprintf(f, "\n");
  va_end(ap);

  return return_value;
}

int _printf(const char *restrict format, ...) {
  if (IS_QUIET)
    return 0;

  va_start(ap, format);
  return_value = print_left_gap(stdout) + vprintf(format, ap);
  va_end(ap);

  return return_value;
}

int _fprintf(fident_t fi, const char *restrict format, ...) {
  static FILE *f;

  if (IS_QUIET)
    return 0;

  f = file_from_fident(fi);

  va_start(ap, format);
  return_value = print_left_gap(f) + vfprintf(f, format, ap);
  va_end(ap);
  
  return return_value;
}

int _sprintf(char *restrict buffer, const char *restrict format, ...) {
  /* NOTE: this function doesn't check quiet trigger since the content is printed at some string
   * buffer. */

  va_start(ap, format);
  return_value = vsprintf(buffer, format, ap);
  va_end(ap);

  return return_value;
}

static inline FILE *file_from_fident(fident_t fi) {
  return (fi == _stdout) ? stdout : stderr;
}

static inline int print_left_gap(FILE *restrict f) {
  int result = (int)left_gap;

  for (unsigned int i = 0; i < left_gap; i++)
    fputc(' ', f);

  left_gap = 0;

  return result;
}

void set_left_gap(unsigned int value) {
  left_gap = value;
}
