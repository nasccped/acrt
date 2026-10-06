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

  if (!(handler = dlopen(so_path, RTLD_NOW))) {
    eprintf("Error: '%s' opening failed (maybe not elf/shared object)!\n", so_path);
    return 0;
  }

  else if (dlinfo(handler, RTLD_DI_LINKMAP, (void **) &link_map) != 0) {
    eprintf("Error: '%s' link map extraction failed!\n", so_path);
    return 0;
  }

  elf_header = (elf_header_t *)link_map->l_addr;

  printf("ELF data:\n");
  printf("  Type: %s\n", elf_header->e_type == ET_DYN ? "dynamic" : "not dynamic");
  printf("  Machine: %u\n", elf_header->e_machine);
  printf("  Entry: 0x%lx\n", (unsigned long)elf_header->e_entry);

  dlclose(handler);
  return 1;
}

void print_usage(char *bin_path) {
  eprintf("This program calls shared object functions at %s.\n\n", CUSTOM_SECTION_NAME);
  eprintf("Usage: %s [.so files...]\n", bin_path);
}
