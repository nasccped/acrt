#ifndef _ACRT_STRING_H_
#define _ACRT_STRING_H_

/* Returns if both strings are lexicographic equals. The return value is:
 * - '1' if equals;
 * - '0' otherwise.
 *
 * Less verbose that 'strcmp(..., ...) == 0'. Note that this function returns '0' if dealing with
 * NULL pointers, but, it'll return '1' if both strings refers to empty strings (zero length). */
int string_equals(const char *s1, const char *s2);

/* Returns if the provided string refers to an whitespace only string. The return value is:
 * - '1' if whitespace only.
 * - '0' otherwise (or when NULL pointer).
 *
 * This function also returns '1' if provided string is empty since it search for ANY CHARACTER
 * that ISN'T whitespace. */
int string_is_whitespace(const char *s);

#endif
