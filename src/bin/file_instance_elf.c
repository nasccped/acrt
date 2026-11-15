#ifdef __ELF__ // Add implementations only for elf compatible machines.

#include "file_instance_elf.h"

// generic header is still required to get instance_init_t definition
#include "file_instance_generic.h"

#include <stdlib.h>
#include <string.h>

elf_instance_t *find_gfi_by_path(int count, elf_instance_t *ei_arr, char *file_path) {
  // for each instance.
  for (int i = 0; i < count; i++) {
    // if name matches.
    if (strcmp(ei_arr[i].file_path, file_path) == 0)
      return &ei_arr[i];
  }

  return NULL;
}

instance_init_t gfi_init(elf_instance_t *ei, char *file_path) {
  instance_init_t init_result = (instance_init_t){ .init_status = II_OK };

  ei->file_path = file_path;
  return init_result;
}

void gfi_deinit(elf_instance_t *ei) {
  ei->file_path = NULL;
  // TODO: add de-initializes...
}

#endif /* __ELF__ */
