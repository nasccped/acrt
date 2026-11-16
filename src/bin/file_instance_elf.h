#ifndef _ACRT_FILE_INSTANCE_ELF_H_
#define _ACRT_FILE_INSTANCE_ELF_H_

#include "file_instance_generic.h"

#include <elf.h>
#include <link.h>

typedef ElfW(Ehdr) elf_header_t;

// Data structure which sets the generic file instance as elf type definitions.
typedef struct generic_file_instance {

  // File path referring to the instance itself.
  char *file_path;

  // Refers to the file length when retrieving data with 'fstat'. Not that the stat.st_size refers
  // to 'off_t' type which means signed integer, but mmap/munmap (which later uses the file length)
  // expects an unsigned mach-arch integer (size_t). Some checks must be done before casting
  // st_size to the actual file length.
  size_t file_size;

  // Mapped memory.
  union {
    void *pointer;
    elf_header_t *eh;
  } mmem;

  // Dynamically loaded object.
  void *dlobj;

} elf_instance_t;

// Data structure referring to the instance initializing status (for elf workflow).
//
// NOTE: this struct can be latter improved avoiding repetitive error kinds of different init
// operations (ie, both open and dlopen provides 'file not found' kind of error).
struct generic_instance_init {

  // Status held by the current instance init.
  enum {
    II_OK,        // Everything is ok.
    II_FDOPENERR, // Failed to open file descriptor.
    II_FSTATERR,  // Failed to get file stat.
    II_STSIZEERR, // st_size not size_t compatible.
    II_MMAPERR,   // Failed to map file.
    II_DLOPENERR  // Failed to dynamically load the object.
  } init_status;

  // Helper info.
  union {

    // Fields related to II_DLOPENERR.
    struct {

      // File path of the loading generated error.
      char *file_path;

      // Error diagnostic string returned by the dlerror function.
      char *diagnostic;

    } dlopenerr;
  };

};

#endif
