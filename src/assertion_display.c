#include "assertion_display.h"
#include <stdio.h>
#include <string.h>

/* Turn literals into string. */
#define __AS_STR(X) #X

#define STRING_OR_PLACEHOLDER(STRING) (STRING) ? (STRING) : "???"

/* Generates a color escape based on a give code. */
#define GEN_COLOR(CODE) "\x1b[" __AS_STR(CODE) "m"

#define RESET GEN_COLOR(0)
#define RED GEN_COLOR(91)
#define GREEN GEN_COLOR(92)
#define BLUE GEN_COLOR(94)
#define WHITE GEN_COLOR(97)

#define FAILED_RED RED "failed" RESET
#define PASSED_GREEN GREEN "passed" RESET

/* Cool tag printing for the provided wrapper inner data. */
void __print_tag(assertion_wrapper_t *);

/* Prints the assertion description based on it's inner kind (boolean expresion,
 * ...). */
void __print_kind_description(assertion_wrapper_t *);

/* Prints the assertion inner values description. */
void __print_result_description(assertion_wrapper_t *);

/* Gets the tag + spaces width (useful for aligned printing result). */
int __get_tag_and_spaces_width(assertion_wrapper_t *);

void display_assertion_wrapper(assertion_wrapper_t *self) {
  if (!self)
    return;
  __print_tag(self);
  __print_kind_description(self);
  __print_result_description(self);
}

void __print_tag(assertion_wrapper_t *self) {
  if (!self)
    return;
  fprintf(self->result ? stdout : stderr, "%s[%s - %s: %d]:%s ", WHITE,
          STRING_OR_PLACEHOLDER(self->group_name),
          STRING_OR_PLACEHOLDER(self->file_name), self->line_number, RESET);
}

void __print_kind_description(assertion_wrapper_t *self) {
  if (!self)
    return;
  FILE *f = self->result ? stdout : stderr;
  const char *colored_status = self->result ? PASSED_GREEN : FAILED_RED;
  switch (self->kind) {
  case BOOLEAN_EXPRESSION:
    fprintf(
        f, "boolean assertion %s for the '" BLUE "%s" RESET "' expression.\n",
        colored_status,
        STRING_OR_PLACEHOLDER(self->data.boolean_expression_representation));
    break;
  }
}

void __print_result_description(assertion_wrapper_t *self) {
  if (!self)
    return;
  FILE *f = self->result ? stdout : stderr;
  switch (self->kind) {
  case BOOLEAN_EXPRESSION:
    // boolean expression don't need to display since they just return
    // bool-like values (0 or 1).
    break;
  }
}

int __get_tag_and_spaces_width(assertion_wrapper_t *self) {
  if (!self)
    return 0;
  int length = strlen(STRING_OR_PLACEHOLDER(self->group_name)) +
               strlen(STRING_OR_PLACEHOLDER(self->file_name)) +
               strlen("[ - : ]: ") + 1;
  for (unsigned int a = 10; self->line_number / a; a *= 10)
    length++;
  return (int)length;
}
