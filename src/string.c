#include "string.h"

#include <string.h>

typedef size_t usize;

int string_equals(const char *s1, const char *s2) {
  usize length;

  // when null or diff length
  if (!s1 || !s2 || (length = strlen(s1)) != strlen(s2))
    return 0;

  for (usize i = 0; i < length; i++) {
    if (s1[i] != s2[i])
      return 0;
  }

  return 1;
}

int string_is_whitespace(const char *s) {
  if (!s)
    return 0;

  for (usize i = strlen(s); i; i--) {
    // WARN: use - 1 since whe using 1 based index.
    switch (s[i - 1]) {

      // if whitespace
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        // just continue
        break;

      // else return false
      default:
        return 0;
    }
  }

  return 1;
}
