#include "assertion_display.h"
#include <stdio.h>
#include <string.h>

#define STRING_OR_PLACEHOLDER(STRING) (STRING) ? (STRING) : "???"

/* Generates a color escape based on a given code. */
#define __GEN_COLOR(CODE) "\x1b[" #CODE "m"

#define RESET __GEN_COLOR(0)
#define RED __GEN_COLOR(91)
#define GREEN __GEN_COLOR(92)
#define YELLOW __GEN_COLOR(93)
#define BLUE __GEN_COLOR(94)
#define WHITE __GEN_COLOR(97)

/* Gets a colored status based on a bool-like result. */
#define COLORED_STATUS(RESULT)                                                 \
  (RESULT) ? GREEN "passed" RESET : RED "failed" RESET

/* Gets the file destionation based on a bool-like result. */
#define GET_FILE(RESULT) (RESULT) ? stdout : stderr

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
  fprintf(self->result ? stdout : stderr,
          WHITE "[%s - %s: " YELLOW "%d" WHITE "]:" RESET " ",
          STRING_OR_PLACEHOLDER(self->group_name),
          STRING_OR_PLACEHOLDER(self->file_name), self->line_number);
}

void __print_kind_description(assertion_wrapper_t *self) {
  if (!self)
    return;
  switch (self->kind) {
  case BOOLEAN_EXPRESSION:
    fprintf(
        GET_FILE(self->result),
        "boolean assertion %s for the '" BLUE "%s" RESET "' expression.\n",
        COLORED_STATUS(self->result),
        STRING_OR_PLACEHOLDER(self->data.boolean_expression_representation));
    break;
  }
}

void __print_result_description(assertion_wrapper_t *self) {
  if (!self)
    return;
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
