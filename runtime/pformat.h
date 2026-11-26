#ifndef _ACRT_RUNTIME_H_
#define _ACRT_RUNTIME_H_

/* This file is intended to provided stdio print and formatting abstractions. This allows an easy
 * print handling since --quiet/--suppress-warnings can disable printing. */

#define _stdout STDOUT_IDENT
#define _stderr STDERR_IDENT

// Adds a bold ansi escape enclosing to the provided string (must be string literal).
#define BOLD_ESCAPE_ENCLOSING(STR) "\x1b[1m" STR "\x1b[0m"

// Shorthand for _fprintf(stderr, ...). Doesn't add newline at the end.
#define PRINTERR(ARGS...) _fprintf(_stderr, ARGS)

// Prints a tagged error message to stderr + adds a newline at the end.
#define TAGERR(ARGS...) do {               \
  PRINTERR("\x1b[91merror\x1b[0m: " ARGS); \
  PRINTERR("\n");                          \
} while (0)

// Works similar to TAGERR but it takes a file_identifier to prints to and uses the 'tip' prefix.
#define TAGTIP(FILE_TARG, ARGS...) do {               \
  _fprintf((FILE_TARG), "\x1b[96mtip\x1b[0m: " ARGS); \
  _fprintf((FILE_TARG), "\n");                        \
} while (0)

// Works similar to TAGTIP but uses the 'warn' prefix. Take a look at __print_tag_warn docs.
#define TAGWARN(FILE_TARG, ARGS...) __print_tag_warn((FILE_TARG), ARGS)

// Type latter used to convert from integer variant to actual FILE pointer.
enum file_identifier {
  STDOUT_IDENT,
  STDERR_IDENT
};

/* These functions bellows are just runtime abstractions for stdio's printers. Note that calling
 * them can cause to reset the default FILE pointer + left_gap. */

/* Prints the warning tag to the specified file. This operation was moved to a single function
 * since warnings can be suppressed by using --suppress-warnings.
 *
 * The returned value means how many bytes were written to the specified file (or 0 if warnings
 * disable).
 *
 * This function also adds a new line at the end of operation (which is also added on the returned
 * value). */
int __print_tag_warn(enum file_identifier, const char *restrict, ...);

/* Prints the format + it's args to the static FILE pointer (stdout by default). The integer being
 * returned means how many bytes were been print to stdout (left gap whitespace inclusive). */
int _printf(const char *restrict, ...);

/* Works the same as _printf function but but takes a file identifier meaning where to print the
 * content (stdout/stderr). */
int _fprintf(enum file_identifier, const char *restrict, ...);

/* Works the same as normal sprintf. Note that this function doesn't adds the left gap whites nor
 * reset it since sprintf conventions must obey the format being used. */
int _sprintf(char *restrict, const char *restrict, ...);

// Set the temporary value to left_gap. The value is reset at the first print called.
void set_left_gap(unsigned int);

#endif
