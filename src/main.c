#include <stdio.h>

int main(int argc, char *argv[]) {
  // avoid comp-fail from -Wall -Werror flags.
  (void)argc;
  (void)argv;

  printf("Let's test it!\n");

  return 0;
}
