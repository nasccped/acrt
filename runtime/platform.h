#ifndef _ACRT_RUNTIME_PLATFORM_H_
#define _ACRT_RUNTIME_PLATFORM_H_

// Add platform specifics.
#if defined(__linux__) || defined(__unix__)
# include "platform_unix.h"
#else
# error "Platform isn't supported (yet). Check the runtime/platform.h!"
#endif

#include <stdbool.h>
#include <stdlib.h>

// Type alias for a platform dependent implementation.
typedef struct platform_struct plat_st;

/* Data structure referring to a single input instance. It contains common fields such as file path
 * string and helper counters, but there's also a platform dependent type alias for platform
 * implementations (since Windows operates different than Unix, and so on). */
typedef struct input_instance_t {

  /* Kind of file path string being used. */
  enum {

    // When the file path is just a pointer to the original string (read-only).
    FPK_UNTOUCHED,

    // When the CURDIR_PREFIX is added to string to be successfully read by functions (ie. dlopen).
    FPK_PREFIXED

  } fp_kind;

  // File path union (used according to 'fp_kind' variant).
  union {

    const char *path_untouched;

    char *path_prefixed;

  };

  // Fields that depends on the platform implementation.
  plat_st plat;

} inp_inst_t;

/* Input type definitions. That's just an abstraction for 'input_instance_t' type since a input
 * object can either be a concrete instance or a reference to an already existing instance. This
 * helps to avoid double-free when terminating the program and loading mess with loaders API. */
typedef struct {

  // Input kind being held.
  enum { IK_REFERENCE, IK_INSTANCE } kind;

  // Self data fields which depends on 'kind'.
  union {

    inp_inst_t

      // Reference to the input instance.
      *refr,

      // Concrete instance.
      inst;
  };

} input_t;

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

/* Define the required de-initializer function for the dependent platform struct. */
void plat_deinit(plat_st *restrict);

/* Define the required initializer function for the dependent platform struct. This function takes
 * a string a file path to the input being initialized. */
bool plat_init(plat_st *restrict, char *);

#endif
