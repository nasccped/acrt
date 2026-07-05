#include <stdio.h>
#include <string.h>

#include "acrt_display.h"

// colorize macros ------------------------------------------------------------

#define BLACK(STR) "\x1b[90m" STR "\x1b[0m"
#define GREEN(STR) "\x1b[92m" STR "\x1b[0m"
#define RED(STR) "\x1b[91m" STR "\x1b[0m"
#define YELLOW(STR) "\x1b[93m" STR "\x1b[0m"

// Returns the status for the current inline assertion (passed or not).
#define CURRENT_RESULT_STATUS(RESULT) (RESULT) ? GREEN("passed") : RED("failed")

// Prints a single line of a result counting table. It takes the header width
// (HW), a file target (FILE: stdout/stderr) and a conditional (boolean) corner
// field (CORNER).
//
// The (CORNER) is used to check if '+' char must be displayed as line
// begins/ends.
#define PRINT_RESULT_COUNTING_TABLE_BORDER(HW, FILE, CORNER)                   \
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

// Prints a single row of a result counting table. It takes a row title (TITLE)
// that is displayed at left side, an integer number (ROW_VALUE) that this title
// holds, a char array that holds this number as string (NUM_HOLDER) and a
// header width.
#define PRINT_RESULT_COUNTING_TABLE_ROW(ROW_TITLE, ROW_VALUE, NUM_HOLDER, HW)  \
  do {                                                                         \
    snprintf((NUM_HOLDER), sizeof((NUM_HOLDER)), "%d", (ROW_VALUE));           \
                                                                               \
    fprintf(f, "| %s: \x1b[90m", (ROW_TITLE));                                 \
                                                                               \
    int mid_gap =                                                              \
        (HW) - 3 - (int)(strlen((ROW_TITLE)) + strlen((NUM_HOLDER)));          \
                                                                               \
    for (int i = 0; i < mid_gap; i++)                                          \
      fputc('-', f);                                                           \
                                                                               \
    fprintf(f, "\x1b[0m %s |\n", (NUM_HOLDER));                                \
  } while (0)

// Returns status for the acrt's 'counting' field.
#define RESULT_COUNTING_STATUS(COUNTING)                                       \
  !(COUNTING)->total   ? BLACK("empty")                                        \
  : (COUNTING)->failed ? RED("failed")                                         \
                       : GREEN("passed")

// Returns a char pointer based on the 'result.kind' field.
#define STRING_FROM_ASSERTION_KIND(KIND)                                       \
  ((KIND) == POINTER_BOOLEAN_ASSERTION_KIND ||                                 \
   (KIND) == SIMPLE_BOOLEAN_ASSERTION_KIND)                                    \
      ? "boolean"                                                              \
      : "undefined"

typedef struct __acrt_counting counting_t;

// Struct used at 'display_counting_data' function. It holds a name/value pair.
struct counting_table_row_pair {

  // The name being used for the current row.
  const char *name;

  // The value pointer.
  const unsigned int *value;
};

void display_acrt_result(result_t *res) {
  if (!res)
    return;

  int passed = res->passed;
  FILE *f = passed ? stdout : stderr;
  const char *ctx_name = res->context.name;
  const char *kind = STRING_FROM_ASSERTION_KIND(res->kind);
  const char *stts = CURRENT_RESULT_STATUS(passed);

  char line[32];
  snprintf(line, sizeof(line), "%d", res->context.line);
  int padding = (int)(strlen(ctx_name) + strlen(line) + 5);

  fprintf(f, "[%s: %s] %s assertion %s\n", ctx_name, line, kind, stts);

  switch (res->kind) {
  case SIMPLE_BOOLEAN_ASSERTION_KIND:
    const char *value_is = passed ? "non zero" : "zero";
    fprintf(f, "%*svalue is %s.\n", padding, "", value_is);
    break;

  case POINTER_BOOLEAN_ASSERTION_KIND:
    fprintf(f, "%*svalue points to %p.\n", padding, "",
            res->data.single_pointer);
    break;
  }
  fprintf(f, "\n");
}

void display_counting_data(const char *ctx_name, counting_t *counting,
                           FILE *f) {
  if (!counting || !f)
    return;

  const char *status = RESULT_COUNTING_STATUS(counting);
  int header_width = (int)(strlen(ctx_name) + strlen(status) + 10);

  PRINT_RESULT_COUNTING_TABLE_BORDER(header_width, f, 1);
  fprintf(f, "| %s assertion status: %s |\n", ctx_name, status);
  PRINT_RESULT_COUNTING_TABLE_BORDER(header_width, f, 0);

  char num[16];
  struct counting_table_row_pair *cur,
      pairs[] = {{.name = "passed", .value = &counting->passed},
                 {.name = "failed", .value = &counting->failed},
                 {.name = "ignored", .value = &counting->ignored},
                 {.name = "total", .value = &counting->total}};

  for (size_t i = 0; i < (sizeof(pairs) / sizeof(pairs[0])); i++) {
    cur = &pairs[i];
    PRINT_RESULT_COUNTING_TABLE_ROW(cur->name, *cur->value, num, header_width);
  }

  PRINT_RESULT_COUNTING_TABLE_BORDER(header_width, f, 1);
}
