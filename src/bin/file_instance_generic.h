#ifndef _ACRT_FILE_INSTANCE_GENERIC_H_
#define _ACRT_FILE_INSTANCE_GENERIC_H_

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

// Return type meaning the gfi initializing status. It can be just refers to 'II_OK' as init status
// (which is the expected) meaning a successfully initialization, or any other enumeration
// accomplished by helper info fields within anonymous struct/unions.
typedef struct {

  enum { II_OK } init_status;

  // NOTE: add helper fields to express the error kind.
  // ...

} instance_init_t;

// Iterates over a generic file instance array searching for a instance that matches the provided
// file path. The returned value is a pointer to that instance (or NULL if not found).
gfi_t *find_gfi_by_path(int count, gfi_t *gfi_arr, char *file_path);

// Initialize the generic instance pointer fields by using the provided pathname. Since the
// initializing can fail, an instance_init state is returned meaning the initialize result. If
// operation somehow fails, the deallocations are done within function's scope.
instance_init_t gfi_init(gfi_t *gfi, char *file_path);

// De-initializes the generic file instance inner fields.
void gfi_deinit(gfi_t *gfi);

#endif
