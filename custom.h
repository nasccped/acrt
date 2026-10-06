#ifndef _CUSTOM_H_
#define _CUSTOM_H_

#define CUSTOM_SECTION_NAME ".custom_section"

#define CUSTOM_FUNCTION(CUSTOM_FUNC_NAME) \
  __attribute__((used, section(CUSTOM_SECTION_NAME))) \
  void CUSTOM_FUNC_NAME(void)

#endif
