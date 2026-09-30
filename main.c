#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("%d args being used:\n", argc - 1);
  for (int i = 1; i < argc; i++)
    printf(" [%d]: %s\n", i - 1, argv[i]);

  return 0;
}
