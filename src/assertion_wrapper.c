#include "assertion_wrapper.h"

assertion_wrapper_t assertion_wrapper_new_bool(const char *file_name,
                                               const unsigned int line_number,
                                               const char *expression,
                                               const char *group_name,
                                               const int result) {
  return (assertion_wrapper_t){.kind = BOOLEAN_EXPRESSION,
                               .file_name = file_name,
                               .line_number = line_number,
                               .group_name = group_name,
                               .result = result,
                               .data.boolean_expression_representation =
                                   expression};
}
