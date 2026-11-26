#ifndef _ACRT_RUNTIME_HELPERS_H_
#define _ACRT_RUNTIME_HELPERS_H_

/* This file provides runtime definitions and macro helpers to avoid out-of-context including
 * between non related source files (turns debugging and implementing little difficulty). */

// acrt's arrays max size (avoid malloc and arena usage).
#define ACRT_BUFFER_MAX_SIZE 64

// Refers to '\0'.
#define NULL_TERM_LEN 1

#if defined(__linux__) || defined(__unix__)
# define CURDIR_PREFIX_STR  "./"
# define ROOTDIR_PREFIX_STR "/"
#else
# error "No default definition(s) for this platform. See helpers.h!"
#endif

#endif
