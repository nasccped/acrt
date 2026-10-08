#include "custom.h"

#include <fcntl.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX(L, R) ((L) > (R)) ? (L) : (R)
#define SYMBOL_TABLE_IDENTIFIER ".symtab"

/* Alias for stderr printing. */
#define eprintf(ARGS...) fprintf(stderr, ARGS)

typedef ElfW(Ehdr) elf_header_t;
typedef ElfW(Shdr) section_header_t;
typedef ElfW(Off) elf_offset_t;
typedef struct stat stat_t;
typedef ElfW(Sym) symbol_table_t;

/* Returns the section name as string. Note that this function requires elf_header and the shstrtab
 * section pointer since initial pointers are necessary for char pointer offset calculation. This
 * function also doesn't handle the 'index' value safety, so, overflowing will obviously crash the
 * program. */
char *get_section_header_name(elf_header_t *elf_header,
                              section_header_t *shstrtab,
                              uint32_t index);
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

char *get_section_header_name(elf_header_t *elf_header,
                              section_header_t *shstrtab,
                              uint32_t index) {
  return (char *)((elf_offset_t)elf_header + shstrtab->sh_offset + (elf_offset_t)index);
}

int handle_so_path(char *so_path) {
  /* fields related to open and mmap. */
  static int file_descriptor;
  static stat_t file_stat;
  static void *file_map;

  /* checks if the asked file is actually an ELF shared object. */
  static int not_dyn_elf;

  static elf_header_t *elf_header;
  static section_header_t *shstrtab;
  static symbol_table_t *symtab;
  static uint64_t symbol_count;

  /* NOTE: we need to use 64 bits for section index storing, so 'custom section not found' can be
   * expressed as -1 without messing with uint32_t most significant bit. */
  static int64_t custom_section_index;

  /* NOTE: Guarantees 32 bits usage since index can be either 'ElfN_Ehdr.e_shstrndx' (uint16_t) or
   * 'ElfN_Shdr.sh_link' (uint32_t) field. */
  static uint32_t shstrtab_index;

  if ((file_descriptor = open(so_path, O_RDONLY)) < 0) {
    eprintf("Error: '%s' file openning failed!\n", so_path);
    return 0;
  }

  else if (fstat(file_descriptor, &file_stat) != 0) {
    eprintf("Error: '%s' stat retrieving failed!\n", so_path);
    close(file_descriptor);
    return 0;
  }

  file_map = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);

  /* descriptor can be safely closed since mmap handles the file data. */
  close(file_descriptor);

  if (file_map == MAP_FAILED) {
    eprintf("Error: '%s' mapping failed!\n", so_path);
    return 0;
  }

  not_dyn_elf = (
    (((long unsigned int)file_stat.st_size) < sizeof(elf_header_t))
    || (memcmp(file_map, ELFMAG, SELFMAG) != 0)
    || (elf_header = (elf_header_t *)file_map)->e_type != ET_DYN
  );

  if (not_dyn_elf) {
    eprintf("Error: '%s' isn't a .ELF shared object!\n", so_path);
    munmap(file_map, file_stat.st_size);
    return 0;
  }

  /* The section header string table index is handled following documentation provided by the elf
   * man pages! */
  switch (elf_header->e_shstrndx) {

    /* SHN_UNDEF means no string table section. */
    case SHN_UNDEF:
      eprintf("Error: '%s' doesn't provide .shstrtab section!\n", so_path);
      munmap(file_map, file_stat.st_size);
      return 0;

    /* SHN_XINDEX means equals/larger than SHN_LORESERVE, so, real index is placed at sh_link field
     * of the first entry of section header table. */
    case SHN_XINDEX:
      shstrtab_index = ((section_header_t *)(
        (elf_offset_t)elf_header + elf_header->e_shoff))->sh_link;
      break;

    default:
      shstrtab_index = (uint32_t)elf_header->e_shstrndx;
      break;
  }

  /* get .shstrtab section ptr. */
  shstrtab = (section_header_t *)(
    (elf_offset_t)elf_header + elf_header->e_shoff
    + ((elf_offset_t)elf_header->e_shentsize * (elf_offset_t)shstrtab_index)
  );

  /* -1 meaning custom section not found. Init aux tables as null. */
  custom_section_index = -1;
  symtab = NULL;

  /* auxiliar variables for section searching. */
  section_header_t *current_section;
  char *section_name;

  for (uint32_t i = 0; i < elf_header->e_shnum; i++) {
    /* when custom section index + symtab already find. */
    if ((custom_section_index >= 0) && symtab)
      break;

    current_section = (section_header_t *)(
      (elf_offset_t)elf_header
      + elf_header->e_shoff
      + (elf_offset_t)(i * elf_header->e_shentsize)
    );
    section_name = get_section_header_name(elf_header, shstrtab, current_section->sh_name);

    /* If current section refers to the custom section. */
    if (strcmp(section_name, CUSTOM_SECTION_NAME) == 0)
      custom_section_index = (int64_t)i;

    /* If current section refers to the symtab section. */
    else if (strcmp(section_name, SYMBOL_TABLE_IDENTIFIER) == 0) {
      symtab = (symbol_table_t *)(
        (elf_offset_t)elf_header
        + current_section->sh_offset
      );
      symbol_count = current_section->sh_size / current_section->sh_entsize;
    }
  }

  /* if custom section not found. */
  if (custom_section_index < 0) {
    printf("Warning: '%s' doesn't provide '%s' on ELF sections!\n", so_path, CUSTOM_SECTION_NAME);
    munmap(file_map, file_stat.st_size);
    return 1;
  }

  /* if symbol table head pointer not found. */
  else if (!symtab) {
    eprintf("Error: couldn't find '%s' section on '%s'!\n", SYMBOL_TABLE_IDENTIFIER, so_path);
    munmap(file_map, file_stat.st_size);
    return 0;
  }

  printf("%s index: %ld\n", CUSTOM_SECTION_NAME, custom_section_index);
  printf("%s head address: %p\n", SYMBOL_TABLE_IDENTIFIER, symtab);
  printf("symbol count: %ld\n", symbol_count);

  munmap(file_map, file_stat.st_size);
  return 1;
}

void print_usage(char *bin_path) {
  eprintf("This program calls shared object functions at %s.\n\n", CUSTOM_SECTION_NAME);
  eprintf("Usage: %s [.so files...]\n", bin_path);
}
