#ifndef _ACRT_LIB_H_
#define _ACRT_LIB_H_

#include "../definitions/acrt_definitions.h"

#define ACRT_BOOL(EXPR) do { \
  if (!_run_boolean_assertion((struct _assertion_context){ __LINE__, #EXPR }, (int)(EXPR))) \
    return; \
} while (0)

#define ACRT_CASE(IDENTIFIER) \
__attribute__((used, section(__ACRT_TEST_CASE_SECTION_NAME))) \
void IDENTIFIER(void)

#endif
