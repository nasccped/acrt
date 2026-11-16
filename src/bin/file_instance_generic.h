#ifndef _ACRT_FILE_INSTANCE_GENERIC_H_
#define _ACRT_FILE_INSTANCE_GENERIC_H_

#include <stdbool.h>
#include <stdlib.h>

// This file provides the interface for generic file input instances, such as:
// - type alias (generic file instance, aka 'gfi_t');
// - common functions:
//   - find instance within array;
//   - create a new instace;
//   - destroy instance;
//   - ...
//
// It doesn't provide API implementation in a common file. It's actually included by platform
// C implementations. Basically:
// - Implement function A for elf compatible as...
// - Implement function A for coff compatible as...
// - and so on...
//
// This allow common source workflow (such as runtime_entrypoint which runs in either unix/windows)
// to call only the type and functions by the generic alias, not the type implementation. For a
// more in depth description, consider reading the instance helper headers and/or implementors,
// named as file_instance_<KIND>.h and file_instance_<KIND>.c, respectively.

// Type alias for the generic instance struct. Note that a 'instance' doesn't means a 'FILE *' or
// 'ElfW(Ehdr)' types. It's actually file (meta)data on 'pre-run'. Things such as:
// - pathname;
// - mapped memory (when using mmap on unix);
// - and other things...
//
// Since the struct isn't defined yet, it isn't recommended to 'field-access' it. Instead, the
// helper functions should be used to initialize, de-initialize and search for instance. The
// implementation is closed to the well defined concrete types.
typedef struct generic_file_instance gfi_t;

// Type meaning the gfi initializing status. This type is just an alias to the real implementation
// which must be done at implementors sources.
typedef struct generic_instance_init gii_t;

// Iterates over a generic file instance array searching for a instance that matches the provided
// file path. The returned value is a pointer to that instance (or NULL if not found).
gfi_t *find_gfi_by_path(int count, gfi_t *gfi_arr, char *file_path);

// Initialize the generic instance pointer fields by using the provided pathname. Since the
// initializing can fail, an generic instance init state is returned meaning the initialize result.
// If operation somehow fails, the deallocations are done within function's scope.
gii_t gfi_init(gfi_t *gfi, char *file_path);

// De-initializes the generic file instance inner fields.
void gfi_deinit(gfi_t *gfi);

// Runs an already initialized generic file instance + returns a boolean meaning the operation
// success.
bool gfi_run(gfi_t *gfi);

// Prints the generic instance init status at stderr. Prints nothing if not an error.
void print_gii(gii_t *gii, size_t flags);

#endif
