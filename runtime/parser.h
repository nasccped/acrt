#ifndef _ACRT_RUNTIME_PARSER_H_
#define _ACRT_RUNTIME_PARSER_H_

#include <stdbool.h>

/* Tries to parse the argc/argv values into the global_cli structure (which is disposed at
 * cli.c) and later accessed by 'extern'. The return type means operation success. The error (if
 * it occurs) is reported at function's scope (or on inner calls). */
bool cli_parse(int, const char **);

#endif
