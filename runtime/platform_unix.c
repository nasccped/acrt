#if defined(__linux__) || defined(__unix__)

#include "helpers.h"
#include "platform_unix.h"
#include "../definitions/acrt_definitions.h"
#include "pformat.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define ARITH_PTR(PTR)      (unsigned char *)(PTR) /* pointer arithmetics */
#define ARITH_NUM(NUM)      (size_t)(NUM)          /* numeric arithmetics */
#define DLOPEN_FAIL         NULL
#define FSTAT_SUCCESS       0
#define MINIMUM_VALID_FD    0
#define TESTCASE_IDENTIFIER __ACRT_TEST_CASE_SECTION_NAME
#define SHINDEX_UNDEF       (uint64_t)(SHN_UNDEF)
#define STRTAB_IDENTIFIER   ".strtab"      /* String table section header name */
#define STRTAB_LONG_NAME    "string table"
#define SYMTAB_IDENTIFIER   ".symtab"      /* Symbol table section header name */
#define SYMTAB_LONG_NAME    "symbol table"

typedef ElfW(Off) offset_t; /* Equivalent of ElfN_Off => uintN_t */
typedef struct platform_dependent unixdep_t;
typedef struct stat stat_t;

/* Returns the N'th section header from the elf header pointer. Note that this function doesn't
 * check index out of bounds. It'll probably try to access unavailable memory and panics with
 * 'SIGSEGV'. So, the invalid accessing must be checked BEFORE passing index to this function. */
static inline sech_t *get_section_header(elfh_t *, size_t);

/* Returns the section header name by taking the elf header + shstrtab section header pointers. We
 * could just redefine this function as just taking the elf + section header index but this forces
 * the required fields (such as shstrndx) re-loading. Passing all preloaded fields is a lot more
 * efficient. */
static inline char *get_section_header_name(sech_t *, elfh_t *, sech_t *);

/* Tries to retrieve the section header number. Note that the returned value can be 0 since
 * SHT_NOBITS allows this. */
static inline offset_t get_shnum(elfh_t *);

/* Returns the shstrtab index as uint64_t. Note that the index is at most an uint32_t type. We're
 * using 64 bits to represent the kind of error (if some). The return value must be latter cast
 * into uint32_t. */
static uint64_t get_shstrndx(elfh_t *);

// Returns if the off_t value is a size_t compatible (required by mmap function).
static inline bool offt_is_sizet_compatible(off_t);

static inline sech_t *get_section_header(elfh_t *eh, size_t index) {
  return (sech_t *)(
    ARITH_PTR(eh) + (ARITH_NUM(eh->e_shoff) + (ARITH_NUM(eh->e_shentsize) * index))
  );
}

static inline char *get_section_header_name(sech_t *sh, elfh_t *eh, sech_t *shstrtab) {
  return (char *)(
    ARITH_PTR(eh) + ARITH_NUM(shstrtab->sh_offset) + ARITH_NUM(sh->sh_name)
  );
}

static inline offset_t get_shnum(elfh_t *eh) {
  return (eh->e_shnum)
    ? (offset_t)(eh->e_shnum)
    : (offset_t)(get_section_header(eh, 0)->sh_size);
}

static uint64_t get_shstrndx(elfh_t *eh) {
  switch (eh->e_shstrndx) {
    case SHN_UNDEF:
      return SHINDEX_UNDEF;

    case SHN_XINDEX:
      return (uint64_t)(
        ((sech_t *)(ARITH_PTR(eh) + (ARITH_NUM(eh->e_shoff))))->sh_link
      );

    default:
      return (uint64_t)eh->e_shstrndx;
  }
}

static inline bool offt_is_sizet_compatible(off_t o) {
  return ((o >= 0) && ((uintmax_t)(o) <= SIZE_MAX));
}

void platdep_deinit(unixdep_t *restrict ud) {

  if (ud->file_size && ud->mdata)
    munmap(ud->mdata, ud->file_size);

  if (ud->dlobj)
    dlclose(ud->dlobj);

  *ud = (unixdep_t){ 0 };
}

bool platdep_init(unixdep_t *restrict ud, char *path) {
  static int fd;
  static stat_t sb;
  static sech_t *temp_sech;
  static char *shname;

  // Section header entry size always 16 bits.
  static uint16_t shentsize;

  /* Section header num initially 16 bits, but can go up to N (32/64) bits if e_shnum
   * equals/greater than SHN_LORESERVE. */
  static offset_t shnum;

  /* shstrtab index initially 16 bits but can go up to 32 bits if e_shstrndx equals/greater than
   * SHN_LORESERVE. */
  static uint32_t shstrndx;

  /* Temporarily holds the section header string index as uint64_t value. The reason of that is
   * better explained at get_shstrndx doc. */
  static uint64_t temp_shstrndx;

  if ((fd = open(path, O_RDONLY)) < MINIMUM_VALID_FD) {
    TAGERR("failed to open file descriptor for '%s'.", path);
    return false;
  }

  else if (fstat(fd, &sb) != FSTAT_SUCCESS) {
    TAGERR("failed to get stats from '%d' file descriptor (%s).", fd, path);
    close(fd);
    return false;
  }

  else if (!offt_is_sizet_compatible(sb.st_size)) {
    TAGERR("stat size (%ld) isn't a size_t compatible value (which is required by mmap).",
           sb.st_size);
    close(fd);
    return false;
  }

  ud->file_size = (size_t)sb.st_size;
  ud->mdata = mmap(NULL, ud->file_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (ud->mdata == MAP_FAILED) {
    TAGERR("'%s' file map failed.", path);
    return false;
  }

  else if ((ud->dlobj = dlopen(path, RTLD_NOW)) == DLOPEN_FAIL) {
    TAGERR("failed to dinamycally load the '%s' file.", path);
    PRINTERR("\n'dlerror' diagnostic:\n"
             " > " BOLD_ESCAPE_ENCLOSING("%s") "\n", dlerror());
    return false;
  }

  else if (!(shentsize = ud->eh->e_shentsize)) {
    TAGERR("'%s' section header entry's size is 0.", path);
    return false;
  }

  else if (!ud->eh->e_shoff) {
    TAGERR("'%s' doesn't have any section headers.", path);
    return false;
  }

  // handle shnum according to elf man page:
  else if (!(shnum = get_shnum(ud->eh))) {
    TAGERR("'%s' doesn't have any section headers.", path);
    return false;
  }

  else if ((temp_shstrndx = get_shstrndx(ud->eh)) == SHINDEX_UNDEF) {
    TAGERR("'%s' doesn't have a section header string table.", path);
    return false;
  }

  shstrndx = (uint32_t)temp_shstrndx;

  ud->shtestcase_ndx = -1;
  ud->shstrtab = get_section_header(ud->eh, (offset_t)shstrndx);

  for (offset_t i = 0; i < shnum; i++) {
    temp_sech = get_section_header(ud->eh, (size_t)i);
    shname = get_section_header_name(temp_sech, ud->eh, ud->shstrtab);

    if (strcmp(shname, STRTAB_IDENTIFIER) == 0)
      ud->strtab = temp_sech;

    else if (strcmp(shname, SYMTAB_IDENTIFIER) == 0)
      ud->symtab = temp_sech;

    else if (strcmp(shname, TESTCASE_IDENTIFIER) == 0)
      ud->shtestcase_ndx = (int)i;

    // break if all sections found.
    if (ud->symtab && ud->strtab && ud->shtestcase_ndx > 0)
      break;
  }

  if (!ud->strtab) {
    TAGERR("couldn't find '%s's %s.", path, STRTAB_LONG_NAME);
    return false;
  }

  else if (!ud->symtab) {
    TAGERR("couldn't find '%s's %s.", path, SYMTAB_LONG_NAME);
    return false;
  }

  return true;
}

bool platdep_run(unixdep_t *restrict ud) {
  if (!ud->testcase.found) {
    TAGWARN(_stdout, "no '%s' section found!", TESTCASE_IDENTIFIER);
    return true;
  }

  // TODO: add actual run.
  return true;
}

#endif
