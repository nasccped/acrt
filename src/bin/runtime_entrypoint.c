#include "cli.h"
#include "cli_flags.h"
#include "print_defs.h"
#include "runtime_entrypoint.h"

// Instance interface.
#include "file_instance_generic.h"

#ifdef __ELF__
  #include "file_instance_elf.h"
#else
// TODO: add COFF compatible interface.
// #include ...
#endif

#include <stdio.h>

// Data struct that manages the input instances.
typedef struct {
  int count;
  gfi_t instance_array[BUFFER_MAX_SIZE];
} instance_manager_t;

bool manager_init(instance_manager_t *im, int count, char **files, fci_t flags);
void manager_deinit(instance_manager_t *im);

bool main_run(cli_t *cli) {
  static instance_manager_t im = { 0 };

  // if initializing failed.
  if (!manager_init(&im, cli->path_count, cli->paths, cli->global_flags)) {
    manager_deinit(&im);
    return false;
  }

  manager_deinit(&im);
  return true;
}

bool manager_init(instance_manager_t *im, int count, char **files, fci_t flags) {
  static gfi_t *instance_ptr, instance_concrete;
  static char *instance_path;

  for (int i = 0; i < count; i++) {

    instance_path = files[i];
    instance_ptr = find_gfi_by_path(im->count, im->instance_array, instance_path);

    if (instance_ptr && !FLAG_INCLUDES(flags, FCI_RERUN)) {
      tagged_err("duplicated file inputs ('%s').\n", instance_path);
      printerr(
        "Passing same file twice isn't allowed by default. That's a safe workflow\n"
        "which prevents consecutive runs, global variables mess up and unexpected behavior!\n"
        "\n"
        "You can still force it by using the '--%s' flag which allows the same object\n"
        "file to be ran two or more times.\n",
        FLN_RERUN
      );
      return false;
    }

    else {
      gfi_init(&instance_concrete, instance_path);
      instance_ptr = &instance_concrete;
    }

    im->instance_array[im->count++] = *instance_ptr;
  }

  return true;
}

void manager_deinit(instance_manager_t *im) {
  // deinit each one of the instances + set values to 0.
  for (int i = 0; i < im->count; i++) {
    gfi_deinit(&im->instance_array[i]);
    im->instance_array[i] = (gfi_t){ 0 };
  }

  im->count = 0;
}
