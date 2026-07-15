#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

#include <stdio.h>
#include <string.h>

// Type abstraction for assertion results.
typedef struct {

  // If it's passed.
  int passed;

} assertion_result_t;

// Test assert function alias.
typedef assertion_result_t (*assert_function_t)();

static char code_location[128];

// Checks if the assertion is passed.
#define ASSERTION_IS_PASSED(ASSERTION) (ASSERTION).passed

// Passed assertion literal.
#define ASSERTION_PASSED (assertion_result_t){.passed = 1}

// Failed assertion literal.
#define ASSERTION_FAILED (assertion_result_t){.passed = 0}

// String placeholder for null pointers.
#define STR_OR_NULL_PLACEHOLDER(STR) (STR) ? (STR) : "<NULL>"

// Set the string content within code_location static var.
#define SET_CODE_LOCATION                                                      \
  snprintf(code_location, sizeof(code_location), "[%s: %d]", __FILE__, __LINE__)

// Returns the string length of code_location static var.
#define CODE_LOCATION_LENGTH (int)strlen(code_location) + 1

// Display the FMT + ARGS at stderr and returns the 'ASSERTION_FAILED' value.
#define DISPLAY_AND_RETURN(FMT, ...)                                           \
  SET_CODE_LOCATION;                                                           \
  fprintf(stderr, "%s " FMT, code_location, __VA_ARGS__);                      \
  return ASSERTION_FAILED;

// Simple boolean assertion.
#define ASSERT(TITLE, RESULT)                                                  \
  if (!(RESULT)) {                                                             \
    DISPLAY_AND_RETURN("%s assertion failed. (line %d)\n", (TITLE), __LINE__); \
  }

// Asserts if two pointers holds the same address.
#define ASSERT_EQ_POINTER(TITLE, EXPECTED, GOT)                                \
  if ((EXPECTED) != (GOT)) {                                                   \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %p, got %p\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ', (EXPECTED), (GOT)); \
  }

// Asserts if two integers are equals.
#define ASSERT_EQ_INT(TITLE, EXPECTED, GOT)                                    \
  if ((EXPECTED) != (GOT)) {                                                   \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %d, got %d\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ', (EXPECTED), (GOT)); \
  }

// Asserts if two strings are the same.
#define ASSERT_EQ_STRING(TITLE, EXPECTED, GOT)                                 \
  if (!(EXPECTED) || !(GOT) || strcmp((EXPECTED), (GOT)) != 0) {               \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %s, got %s\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ',                     \
                       STR_OR_NULL_PLACEHOLDER(EXPECTED),                      \
                       STR_OR_NULL_PLACEHOLDER(GOT));                          \
  }

// Asserts if a function that compares two undefined type values returns true
// ('1') by taking these two values.
//
// Also expects a 'TO_STR_FUNC' that converts the value provided to a readable
// string.
#define ASSERT_EQ_CUSTOM(TITLE, EXPECTED, GOT, EQ_FUNC, TO_STR_FUNC)           \
  if (!EQ_FUNC((EXPECTED), (GOT))) {                                           \
    DISPLAY_AND_RETURN("%s assertion failed.\n%*cExpecting %s, got %s\n",      \
                       (TITLE), CODE_LOCATION_LENGTH, ' ',                     \
                       TO_STR_FUNC((EXPECTED)), TO_STR_FUNC((GOT)));           \
  }

// Macro that expects a set of valid 'assert_function_t' function pointers. It
// automatically creates and run them.
//
// Can conflict with naming scope.
#define RUN_TESTS(...)                                                         \
  do {                                                                         \
    assert_function_t current, test_functions[] = {__VA_ARGS__};               \
    size_t total_amount_of_funtions =                                          \
        (sizeof(test_functions) / sizeof(test_functions[0]));                  \
                                                                               \
    for (size_t i = 0; i < total_amount_of_funtions; i++) {                    \
      current = test_functions[i];                                             \
                                                                               \
      if (!ASSERTION_IS_PASSED(current()))                                     \
        return 1;                                                              \
    }                                                                          \
  } while (0)

#endif
