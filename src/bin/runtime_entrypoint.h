#ifndef _ACRT_RUNTIME_ENTRYPOINT_H_
#define _ACRT_RUNTIME_ENTRYPOINT_H_

#include "cli.h"

#include <stdbool.h>
#include <stdlib.h>

// Does the runtime entrypoint main run.
bool main_run(cli_t *cli);

#endif
