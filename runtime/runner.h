#ifndef _ACRT_RUNTIME_RUNNER_H_
#define _ACRT_RUNTIME_RUNNER_H_

#include "parser.h"

/* Runs the application based on the cli inner fields. The boolean value being returned means the
 * operation success. */
bool run(cli_t *restrict);

#endif
