#include "../src/acrt.h"

// Returns if given number is prime.
int is_prime(int number);

// New acrt with a custom context name.
static acrt_t my_acrt();

int main() {
  acrt_t acrt = my_acrt();

  // A custom context name is being used:
  ACRT_BOOL(&acrt, is_prime(4));

  return 0;
}

int is_prime(int number) {
  if (number < 2)
    return 0;

  for (int i = 2; i < number; i++) {
    if ((number % i) == 0)
      return 0;
  }
  return 1;
}

static acrt_t my_acrt() {
  acrt_t acrt = ACRT_DEFAULT;

  acrt_set_context_name_to_custom(&acrt, "cool assertions");

  return acrt;
}
