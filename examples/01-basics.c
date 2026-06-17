#include "../src/acrt.h"
#include <stdio.h>

int main(void) {
  // creates a new acrt
  acrt_t acrt = acrt_new();

  printf("Let's assert integers\n");

  // assert integers
  for (int i = 1; i < 4; i++) {
    printf(" > asserting %d: ", i);
    acrt_bool(&acrt, i);
    printf("\x1b[92mok\x1b[0m\n");
  }

  // assert pointer
  char *string = "my string";
  printf("\nLet's assert a char pointer\n");
  printf(" > asserting '%s': ", string);
  acrt_bool(&acrt, string);
  printf("\x1b[92mok\x1b[0m\n\n");

  // fail assert
  int fail = 0;
  printf("Let's do a failable assertion (%d)\n", fail);
  acrt_bool(&acrt, fail);

  // the call bellow won't be triggered since the fail panics the program. try
  // switching the 'fail' variable to 1 and see what happens.
  printf("Was this line printed?\n");

  return 0;
}
