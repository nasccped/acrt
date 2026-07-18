#include <stdio.h>

#include "../src/acrt.h"

#define MIN 2
#define MAX 15

static acrt_t acrt = ACRT_DEFAULT;
static int integer = 42;
static char *string = "some string";

// Function callback for integers.
void int_callback(int *integer);

// Returns if a given integer is prime or not.
int is_prime(int n);

// Function callback for string.
void string_callback(char *string);

int main() {
  // NOTE: allow code compile since warnings are turned into error.
  if (string || integer)
    printf("%s", "");

  // NOTE: uncomment a single of this function.
  // acrt_set_on_fail_continue_assertions(&acrt);
  // acrt_set_on_fail_disable_assertions(&acrt);
  // acrt_set_on_fail_exit_program(&acrt, 2);
  // acrt_set_on_fail_print_counting_and_exit(&acrt, 3);
  // acrt_set_on_fail_run_callback_and_exit(&acrt, (void (*)(void*))int_callback, &integer, 4);
  // acrt_set_on_fail_run_callback_without_exit(&acrt, (void (*)(void*))string_callback, string, 1);

  printf("Let's assert how many integer are ");
  printf("prime from %d to %d!\n\n", MIN, MAX);

  for (int i = 2; i < 15; i++)
    ACRT_BOOL(&acrt, is_prime(i));

  printf("\nAssertions done!\n");

  return 0;
}

void int_callback(int *integer) {
  printf("Integer callback called with %d\n", *integer);
}

int is_prime(int n) {
  if (n < 2)
    return 0;

  for (int i = 2; i < n; i++) {
    if (n % i == 0)
      return 0;
  }

  return 1;
}

void string_callback(char *string) {
  printf("String callback called with %s\n", string);
}
