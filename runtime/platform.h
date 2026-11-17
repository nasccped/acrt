#ifndef _ACRT_RUNTIME_PLATFORM_H_
#define _ACRT_RUNTIME_PLATFORM_H_

#include <stdbool.h>
#include <stdlib.h>

/* Actually runs the application on a platform specific way (load and run input instances). It
 * takes:
 * - an integer (how many inputs are we dealing with);
 * - a string array (input paths);
 * - size_t (global flags being used).
 *
 * The returned value is a boolean meaning the operation success.
 *
 * Even platform specific implementations consist on common types, so, this function is implemented
 * at platform.c generic file but this one deals with platform specific headers and definitions. */
bool platform_run(int, const char **, size_t);

#endif
