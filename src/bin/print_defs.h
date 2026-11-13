#ifndef _ACRT_PRINT_DEFS_H_
#define _ACRT_PRINT_DEFS_H_

// This file is reserved for printing and formatting definitions to be used across C sources
// without repeted redefinitions.

// Prints directly to stderr.
#define printerr(ARGS...) fprintf(stderr, ARGS)

// Prints directly to stderr with an error tag.
#define tagged_err(ARGS...) printerr("\x1b[91merror\x1b[0m: " ARGS)

#endif
