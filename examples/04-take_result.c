#include "../src/acrt.h"
#include <stdio.h>

acrt_t my_acrt();

int is_prime(int);

int main(void) {
  acrt_t acrt = my_acrt();
  printf("Let's assert 2..9 primes:\n\n");
  for (int i = 2; i <= 9; i++) {
    printf("number %d is ", i);
    if (acrt_bool(acrt, is_prime(i)))
      printf("a \x1b[92mprime\x1b[0m\n");
    else
      printf("\x1b[91mnot a prime\x1b[0m\n");
  }
  return 0;
}

acrt_t my_acrt() {
  acrt_t acrt = acrt_new();
  acrt_set_name(&acrt, "taking result");
  acrt_set_display_variant(&acrt, NO_ASSERTION_DISPLAY);
  acrt_set_on_fail(&acrt, CONTINUE_ASSERTIONS);
  return acrt;
}

int is_prime(int x) {
  if (x < 2)
    return 0;
  for (int i = 2; i < x; i++) {
    if (!(x % i))
      return 0;
  }
  return 1;
}
