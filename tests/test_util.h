#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

#define ASSERTION_PASSED 1
#define ASSERTION_FAILED 0

#define NULL_PLACEHOLDER "<NULL>"

static char code_location[128];

#define SET_CODE_LOCATION                                                      \
  snprintf(code_location, sizeof(code_location), "[%s: %d]", __FILE__, __LINE__)

#define CODE_LOCATION_LENGTH (int)strlen(code_location) + 1

#define DISPLAY_AND_RETURN(FMT, ...)                                           \
  SET_CODE_LOCATION;                                                           \
  fprintf(stderr, "%s " FMT, code_location, __VA_ARGS__);                      \
  return ASSERTION_FAILED;

#define ASSERT_EQ_WITH_STRING_FUNC(TITLE, EXPECTED, GOT, STR_FUNC)             \
  if ((EXPECTED) != (GOT)) {                                                   \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %s, got %s\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ',                     \
                       STR_FUNC((EXPECTED)), STR_FUNC((GOT)));                 \
  }

#define ASSERT_EQ_POINTER(TITLE, EXPECTED, GOT)                                \
  if ((EXPECTED) != (GOT)) {                                                   \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %p, got %p\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ', (EXPECTED), (GOT)); \
  }

#define ASSERT_EQ_INT(TITLE, EXPECTED, GOT)                                    \
  if ((EXPECTED) != (GOT)) {                                                   \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %d, got %d\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ', (EXPECTED), (GOT)); \
  }

#define ASSERT_EQ_STRING(TITLE, EXPECTED, GOT)                                 \
  if (strcmp((EXPECTED), (GOT)) != 0) {                                        \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %s, got %s\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ', (EXPECTED), (GOT)); \
  }

#endif
