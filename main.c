#include "custom.h"

#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Alias for stderr printing. */
#define eprintf(ARGS...) fprintf(stderr, ARGS)

typedef struct link_map link_map_t;
typedef ElfW(Ehdr) elf_header_t;
typedef ElfW(Shdr) section_header_t;

int handle_so_path(char *so_path);
void print_usage(char *bin_path);

int main(int argc, char *argv[]) {
  static int exit_code = EXIT_SUCCESS;

  if (argc < 2) {
    eprintf("Bad usage!\n\n");
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; i++) {
    if (!handle_so_path(argv[i]))
      exit_code = EXIT_FAILURE;
  }

  return exit_code;
}

int handle_so_path(char *so_path) {
  static void *handler;
  static link_map_t *link_map;
  static elf_header_t *elf_header;
  static section_header_t *sh_strtab;

  /* NOTE: Guarantees 32 bits usage since index can be either 'ElfN_Ehdr.e_shstrndx' (uint16_t) or
   * 'ElfN_Shdr.sh_link' (uint32_t) field. */
  static uint32_t sh_strtab_index;

  if (!(handler = dlopen(so_path, RTLD_NOW))) {
    eprintf("Error: '%s' opening failed (maybe not elf/shared object)!\n", so_path);
    return 0;
  }

  else if (dlinfo(handler, RTLD_DI_LINKMAP, (void **) &link_map) != 0) {
    eprintf("Error: '%s' link map extraction failed!\n", so_path);
    return 0;
  }

  elf_header = (elf_header_t *)link_map->l_addr;

  /* The section header string table index is handled following documentation provided by the elf
   * man pages! */
  switch (elf_header->e_shstrndx) {

    /* Undefined index means no string table section. */
    case SHN_UNDEF:
      eprintf("Error: '%s' .so file doesn't provides '.shstrtab' section!\n", so_path);
      return 0;

    /* SHN_XINDEX means equals/larger than SHN_LORESERVE, so, real index is placed at sh_link field
     * of the first entry of section header table. */
    case SHN_XINDEX:
      sh_strtab_index = ((section_header_t *)(elf_header + elf_header->e_shoff))->sh_link;
      break;

    default:
      sh_strtab_index = (uint32_t)elf_header->e_shstrndx;
      break;
  }

  sh_strtab = (section_header_t *)(elf_header
                                  + elf_header->e_shoff
                                  + (sh_strtab_index * elf_header->e_shentsize));

  printf("Section header string table address: %p\n", sh_strtab);

  dlclose(handler);
  return 1;
}

void print_usage(char *bin_path) {
  eprintf("This program calls shared object functions at %s.\n\n", CUSTOM_SECTION_NAME);
  eprintf("Usage: %s [.so files...]\n", bin_path);
}
