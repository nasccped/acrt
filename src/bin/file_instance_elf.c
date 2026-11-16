#ifdef __ELF__ // Add implementations only for elf compatible machines.

#include "cli.h"
#include "cli_flags.h"
#include "file_instance_elf.h"
#include "print_defs.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Later used to check if the input file is prefixed by the 'CURDIR' string.
#define CURDIR_PREFIX "./"

elf_instance_t *find_gfi_by_path(int count, elf_instance_t *ei_arr, char *file_path) {
  // for each instance.
  for (int i = 0; i < count; i++) {
    // if name matches.
    if (strcmp(ei_arr[i].file_path, file_path) == 0)
      return &ei_arr[i];
  }

  return NULL;
}

gii_t gfi_init(elf_instance_t *ei, char *file_path) {
  static int file_descriptor;
  static struct stat file_stat;
  static gii_t init_result;

  ei->file_path = file_path;
  init_result = (gii_t){ .init_status = II_OK };

  // when opening fails.
  if ((file_descriptor = open(ei->file_path, O_RDONLY)) < 0) {
    init_result.init_status = II_FDOPENERR;
    gfi_deinit(ei);
    return init_result;
  }

  // when file descriptor opening fails.
  else if ((fstat(file_descriptor, &file_stat)) != 0) {
    init_result.init_status = II_FSTATERR;
    gfi_deinit(ei);
    return init_result;
  }

  // when file size can't be represented as size_t (required by mmap function).
  else if (file_stat.st_size < 0 || (uintmax_t) file_stat.st_size > SIZE_MAX) {
    init_result.init_status = II_STSIZEERR;
    gfi_deinit(ei);
    return init_result;
  }

  ei->file_size = (size_t)file_stat.st_size;
  ei->mmem.pointer = mmap(NULL, ei->file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);

  // file descriptor no longer necessary.
  close(file_descriptor);

  // if mapping failed.
  if (ei->mmem.pointer == MAP_FAILED) {
    init_result.init_status = II_MMAPERR;
    gfi_deinit(ei);
    return init_result;
  }

  // if dynamic loading failed.
  else if (!(ei->dlobj = dlopen(ei->file_path, RTLD_NOW))) {
    init_result.init_status = II_DLOPENERR;
    init_result.dlopenerr.diagnostic = dlerror();
    init_result.dlopenerr.file_path = ei->file_path;
    gfi_deinit(ei);
    return init_result;
  }

  return init_result;
}

void gfi_deinit(elf_instance_t *ei) {
  // unmap only when well know size/addr.
  if (ei->mmem.pointer && ei->file_size)
    munmap(ei->mmem.pointer, ei->file_size);

  // if dynamic object still opened.
  if (ei->dlobj)
    dlclose(ei->dlobj);

  ei->file_path = NULL;
  ei->file_size = 0;
  ei->mmem.pointer = NULL;
  ei->mmem.eh = NULL;
  ei->dlobj = NULL;
}

bool gfi_run(gfi_t *gfi) {
  // TODO: add actual run
  (void)gfi;
  printf("'%s' cases are ready to run!\n", gfi->file_path);

  return true;
}

void print_gii(gii_t *gii, fci_t flags) {
  static bool str_prefix_matches;

  switch (gii->init_status) {

    case II_OK:
      break;

    case II_FDOPENERR:
      printerr("Couldn't open file descriptor.\n");
      break;

    case II_FSTATERR:
      printerr("Couldn't get file stat.\n");
      break;

    case II_STSIZEERR:
      printerr("File stat size isn't 'size_t' compatible.\n");
      break;

    case II_MMAPERR:
      printerr("Couldn't map file descriptor to pointer.\n");
      break;

    case II_DLOPENERR:
      str_prefix_matches = (bool)(
        strncmp(CURDIR_PREFIX, gii->dlopenerr.file_path, strlen(CURDIR_PREFIX)) == 0
      );

      printerr("Couldn't dynamically open the file. 'dlerror' diagnostic:\n"
               "   " INIT_ERROR_ESCAPE(" > %s ") "\n", gii->dlopenerr.diagnostic);

      // print missing prefix ('./') if verbose.
      if (!str_prefix_matches && FLAG_INCLUDES(flags, FCI_VERBOSE)) {
        printerr("\n");
        tagged_tip(stderr, "may missing '%s' prefix(?).\n", CURDIR_PREFIX);
      }

      break;
  }
}

#endif /* __ELF__ */
