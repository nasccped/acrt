#ifndef _ACRT_RUNTIME_PLATFORM_H_
#define _ACRT_RUNTIME_PLATFORM_H_

#include <stdbool.h>

// Struct later defined at platform specific header definitions.
typedef struct platform_dependent platdep_t;

// De-initializes the platform dependent inner fields.
void platdep_deinit(platdep_t *restrict);

/* Initializes the platform dependent inner fields according to the passed file path. 'false' is
 * returned on fail case and the error reason must be print at it's function scope. */
bool platdep_init(platdep_t *restrict, char *);

// Runs the platform dependent struct.
bool platdep_run(platdep_t *restrict);

#endif
