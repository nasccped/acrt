#include "../src/acrt.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "This program expects to receive arguments.\n");
    fprintf(stderr, "Aborting!\n");
    return 1;
  }
  acrt_t acrt = acrt_new();
  acrt_set_display_variant(&acrt, DISPLAY_ALL_ASSERTIONS);
  for (int i = 1; i < argc; i++) {
    acrt_set_name(&acrt, argv[i]);
    acrt_bool(acrt, strcmp(acrt.name, argv[i]) == 0);
  }
  return 0;
}
