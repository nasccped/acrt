#include <stdio.h>

/* Alias for stderr printing. */
#define eprintf(ARGS...) fprintf(stderr, ARGS)

void print_usage(void);

int main(int argc, char *argv[]) {
  static int func_count;
  static char *so_path, **functions;

  if (argc < 2) {
    eprintf("Bad usage!\n\n");
    print_usage();
    return 1;
  }

  so_path = argv[1];
  functions = argv + 2;
  func_count = argc - 2;

  printf("Calling %d functions from %s:\n", func_count, so_path);

  for (int i = 0; i < func_count; i++)
    printf(" [%d] %s\n", i, functions[i]);

  return 0;
}

void print_usage(void) {
  eprintf("This program calls a shared object's functions.\n\n");
  eprintf("Usage: <SO_PATH> [func1 func2 ...]\n");
}
