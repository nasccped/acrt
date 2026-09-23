#ifndef _ACRT_MACRO_UTILS_H_
#define _ACRT_MACRO_UTILS_H_

/* This header provides common macros that are used across code base to avoid repetitions.
 *
 * WARN: we don't include any header here (even when using header-defined functions). This leads to
 * compile error due to 'undefined-function-call' or something... You should remember to include
 * all the necessary code at the source implementer (C file). */

/* Converts the provided code into an ansi code escape string. Note that this macro doesn't check
 * code validity, so you must ensure what code you're using (or just define it once and use a
 * single definition all code ahead). */
#define __TO_COLOR_ESCAPE(CODE) "\x1b[" #CODE "m"

#define RESET_ESCAPE  __TO_COLOR_ESCAPE(0)
#define BOLD_ESCAPE   __TO_COLOR_ESCAPE(1)
#define ITALIC_ESCAPE __TO_COLOR_ESCAPE(3)
#define RED_ESCAPE    __TO_COLOR_ESCAPE(91)
#define GREEN_ESCAPE  __TO_COLOR_ESCAPE(92)

/* Wraps all the statements into a single 'do while' block. This can provides a better usage
 * ergonomics such as allowing function-like syntax. If clause followed by single block statement,
 * and so on... */
#define __SINGLE_BLOCK(STATEMENTS) do { STATEMENTS } while (0)

/* Shorthand for 'fprintf(stderr, ...)'. Avoid std(out/err) printing misuse with a print_err
 * dedicated macro. This macro also flushes the stderr file to ensure content printing before
 * forced exit or user input.
 *
 * The usage rules follows 'printf' function signature which expects:
 * - first argument to be a string literal used as format for the following args;
 * - following args being type compatible with the provided format specifier ('%s', '%d', ...). */
#define eprint(ARGS...) __SINGLE_BLOCK(fprintf(stderr, ARGS); fflush(stderr);)

/* Does the same as 'eprint' macro but adds a newline at the end of printing. It also flushes the
 * stderr (done at 'eprint' macro expansion). */
#define eprintln(ARGS...) __SINGLE_BLOCK(eprint(ARGS); eprint("\n");)

/* Prints the provide content to stdout with a newline at the end. The usage rules follows the
 * 'printf' function signature (already explained at 'eprint' macro) and the stdout is flush at the
 * end of expansion. */
#define println(ARGS...) __SINGLE_BLOCK(printf(ARGS); printf("\n"); fflush(stdout);)

/* Prints the error tag followed by a message to stderr. */
#define error_tag_println(ARGS...) eprintln(RED_ESCAPE "error" RESET_ESCAPE ": " ARGS)

/* Refers to an unreachable program state (generally associated with my bad coding). This macro
 * just prints the unreachable message and exits the current process with '1' status code. */
#define unreachable(ARGS...) __SINGLE_BLOCK(                                       \
    eprint(RED_ESCAPE "unreachable" RESET_ESCAPE "[%s:%d]: ", __FILE__, __LINE__); \
    eprintln(ARGS);                                                                \
    exit(1);                                                                       \
)

/* Checks if the provide ptr is null (or zero - any bool compatible is valid). If so, just report
 * the unreachable value and quits. */
#define NON_NULL_OR_UNREACHABLE(PTR) __SINGLE_BLOCK( \
  if (!(PTR))                                                                                 \
    unreachable("%s'" #PTR "'%s points to <null> (or is zero)", ITALIC_ESCAPE, RESET_ESCAPE); \
)

#endif
