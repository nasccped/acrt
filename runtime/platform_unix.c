#if defined(__linux__) || defined(__unix__)

#include "helpers.h"
#include "platform_unix.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define DLOPEN_FAIL      NULL
#define FSTAT_SUCCESS    0
#define MINIMUM_VALID_FD 0

typedef struct platform_dependent unixdep_t;
typedef struct stat stat_t;

// Returns if the off_t value is a size_t compatible (required by mmap function).
static inline bool offt_is_sizet_compatible(off_t);

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

  return true;
}

bool platdep_run(unixdep_t *restrict ud) {
  (void)ud;
  // TODO: add run.
  return true;
}

#endif
