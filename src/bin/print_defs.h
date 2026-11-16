#ifndef _ACRT_PRINT_DEFS_H_
#define _ACRT_PRINT_DEFS_H_

// This file is reserved for printing and formatting definitions to be used across C sources
// without repeted redefinitions.

// Enclose the passed string with an instance init error kind of style.
#define INIT_ERROR_ESCAPE(STR) "\x1b[1;97;41m" STR "\x1b[0m"

// Prints directly to stderr.
#define printerr(ARGS...) fprintf(stderr, ARGS)

// Prints directly to stderr with an error tag.
#define tagged_err(ARGS...) printerr("\x1b[91merror\x1b[0m: " ARGS)

// Macro alias for 'tip:' kind of message. It expects a file target to print (stdout/stderr)
// followed by the fmt + arguments.
#define tagged_tip(FILE_TARG, ARGS...) fprintf((FILE_TARG), "\x1b[96mtip\x1b[0m: " ARGS)

#endif
