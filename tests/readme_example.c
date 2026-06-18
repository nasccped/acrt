#include "../src/acrt.h"
#include <stdlib.h>

int main() {
  acrt_t acrt = acrt_new();
  acrt_bool(acrt, 1);
  acrt_bool(acrt, 2 != 0);
  char *s = "hello";
  acrt_bool(acrt, s);
  s = NULL;
  acrt_bool(acrt, !s);
  return 0;
}
