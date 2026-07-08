/* ****************************************************************************
 * MIT License
 *
 * Copyright (c) 2026 nasccped
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ************************************************************************* */

#include <stdio.h>
#include <string.h>

#include "acrt_display.h"

// colorize macros ------------------------------------------------------------

#define BLACK(STR) "\x1b[90m" STR "\x1b[0m"
#define GREEN(STR) "\x1b[92m" STR "\x1b[0m"
#define RED(STR) "\x1b[91m" STR "\x1b[0m"
#define YELLOW(STR) "\x1b[93m" STR "\x1b[0m"

// Returns a string based on the 'kind' field.
#define ASSERTION_KIND_STRING(KIND)                                            \
  ((KIND) == POINTER_BOOLEAN_ASSERTION_KIND ||                                 \
   (KIND) == INTEGER_BOOLEAN_ASSERTION_KIND)                                   \
      ? "boolean"                                                              \
      : "undefined"

// Returns a string that represents the 'acrt_result.status' field.
#define ASSERTION_STATUS_STRING(STTS)                                          \
  (STTS) == PASSED_ASSERTION_WITHOUT_WARNING ? GREEN("passed")                 \
  : (STTS) == PASSED_ASSERTION_WITH_WARNING                                    \
      ? GREEN("passed") " with " YELLOW("warning")                             \
  : (STTS) == FAILED_ASSERTION ? RED("failed")                                 \
                               : YELLOW("ignored")

// Prints the top/mid/bottom border line of a counting table. It takes a file to
// display (stdout/stderr), the header with (HW) and a boolean (integer) meaning
// if there's corners.
#define PRINT_RESULT_COUNTING_TABLE_BORDER(FILE, HW, CORNER)                   \
  do {                                                                         \
    int maximum = (HW) + 4;                                                    \
                                                                               \
    for (int i = 0; i < maximum; i++) {                                        \
      if ((CORNER) && (i == 0 || i == (maximum) - 1))                          \
        fputc('+', (FILE));                                                    \
      else if (i == 0 || i == (maximum) - 1)                                   \
        fputc('|', (FILE));                                                    \
      else if (!(CORNER) && (i == 1 || i == (maximum) - 2))                    \
        fputc(' ', (FILE));                                                    \
      else                                                                     \
        fputc('-', (FILE));                                                    \
    }                                                                          \
                                                                               \
    fprintf((FILE), "\n");                                                     \
  } while (0)

// Prints a single row of a result counting table. It takes a file to display
// (stdout/stderr), the title of current row (text displayed on the left side),
// the integer value held by the meaning of this title and a header width (HW).
#define PRINT_RESULT_COUNTING_TABLE_ROW(FILE, ROW_TITLE, ROW_VALUE, HW)        \
  do {                                                                         \
    char numstr[16];                                                           \
    snprintf(numstr, sizeof(numstr), "%d", (ROW_VALUE));                       \
    int mid_gap = (HW) - 3 - (int)(strlen((ROW_TITLE)) + strlen(numstr));      \
                                                                               \
    fprintf((FILE), "| %s: \x1b[90m", (ROW_TITLE));                            \
                                                                               \
    for (int i = 0; i < mid_gap; i++)                                          \
      fputc('-', (FILE));                                                      \
                                                                               \
    fprintf((FILE), "\x1b[0m %s |\n", numstr);                                 \
  } while (0)

// Returns status string for the acrt's 'counting' field.
#define RESULT_COUNTING_STATUS(COUNTING)                                       \
  !(COUNTING)->total                 ? BLACK("empty")                          \
  : (COUNTING)->failed               ? RED("failed")                           \
  : (COUNTING)->passed.with_warnings ? "partially " GREEN("passed")            \
                                     : GREEN("passed")

typedef struct __acrt_counting counting_t;

void display_acrt_result(acrt_result_t *res) {
  if (!res)
    return;

  // string helding the current code line
  char line[32];
  const char
      // result context name
      *ctx_name,
      // assertion kind as str
      *kind,
      // assertion status
      *stts;
  int
      // is assertion passed?
      passed,
      // left padding calc.
      padding;
  // file to display
  FILE *f;

  passed = acrt_result_is_passed(res);
  f = passed ? stdout : stderr;
  ctx_name = res->context.name;
  kind = ASSERTION_KIND_STRING(res->kind);
  stts = ASSERTION_STATUS_STRING(res->status);

  snprintf(line, sizeof(line), "%d", res->context.line);
  padding = (int)(strlen(ctx_name) + strlen(line) + 5);

  fprintf(f, "[%s: %s] %s assertion %s\n", ctx_name, line, kind, stts);

  switch (res->kind) {
  case INTEGER_BOOLEAN_ASSERTION_KIND:
    fprintf(f, "%*svalue was cast to %d.\n", padding, "",
            res->data.integer_cast);
    break;

  case POINTER_BOOLEAN_ASSERTION_KIND:
    fprintf(f, "%*svalue points to %p.\n", padding, "",
            (void *)res->data.single_pointer);
    break;
  }
  fprintf(f, "\n");
}

// Macro alias for 'PRINT_RESULT_COUNTING_TABLE_ROW' call.
#define PRCTR(FILE, TITLE, VALUE, HW)                                          \
  PRINT_RESULT_COUNTING_TABLE_ROW((FILE), (TITLE), (VALUE), (HW))

void display_counting_data(const char *ctx_name, counting_t *counting,
                           FILE *f) {
  if (!counting || !f)
    return;

  // status string for the current counting
  const char *status;

  unsigned int
      // total of assertions passed with warnings
      pww,
      // total of assertion passed with no warnings
      pwnw,
      // total of failed assertions
      fld,
      // totla of ignored assertions
      ignd,
      // total of assertions
      ttl;

  // header width
  int hw;

  status = RESULT_COUNTING_STATUS(counting);
  hw = (int)(strlen(ctx_name) + strlen(status) + 10);

  PRINT_RESULT_COUNTING_TABLE_BORDER(f, hw, 1);
  fprintf(f, "| %s assertion status: %s |\n", ctx_name, status);
  PRINT_RESULT_COUNTING_TABLE_BORDER(f, hw, 0);

  pww = counting->passed.with_warnings;
  pwnw = counting->passed.without_warnings;
  fld = counting->failed;
  ignd = counting->ignored;
  ttl = counting->total;

  PRCTR(f, "passed (without warnings)", pwnw, hw);
  PRCTR(f, "passed (with warnings)", pww, hw);
  PRCTR(f, "failed", fld, hw);
  PRCTR(f, "ignored", ignd, hw);
  PRCTR(f, "total", ttl, hw);
  PRINT_RESULT_COUNTING_TABLE_BORDER(f, hw, 1);
}
