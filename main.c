#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

/* Alias for stderr printing. */
#define eprintf(ARGS...) fprintf(stderr, ARGS)

void print_usage(void);

int main(int argc, char *argv[]) {
  static int func_count, exit_code;
  static char *so_path, **functions;
  static void *so_handler, (*func_ptr)(void);

  exit_code = EXIT_SUCCESS;

  if (argc < 2) {
    eprintf("Bad usage!\n\n");
    print_usage();
    return EXIT_FAILURE;
  }

  so_path = argv[1];
  functions = argv + 2;
  func_count = argc - 2;

  if (!(so_handler = dlopen(so_path, RTLD_NOW))) {
    eprintf("Failed to open %s handler!\n", so_path);
    return EXIT_FAILURE;
  }

  printf("Calling %d functions from %s:\n", func_count, so_path);

  for (int i = 0; i < func_count; i++) {
    printf(" >>> [%d] %s", i, functions[i]);

    if (!(func_ptr = (void (*)(void)) dlsym(so_handler, functions[i]))) {
      printf(" failed to open.\n");
      exit_code |= EXIT_FAILURE;
      continue;
    } else {
      printf("\n");
    }

    func_ptr();
  }

  dlclose(so_handler);

  return exit_code;
}

void print_usage(void) {
  eprintf("This program calls a shared object's functions.\n\n");
  eprintf("Usage: <SO_PATH> [func1 func2 ...]\n");
}
