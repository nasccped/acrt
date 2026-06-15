#include "../src/acrt.h"
#include <stdio.h>

int main() {
  AssertManager *man = assert_manager_new("AssertManager");
  printf("manager name: %s\n", man->manager_name);
  assert_manager_set_name(man, "cool name");
  printf("new manager name: %s\n", man->manager_name);
  assert_manager_destroy(man);
  return 0;
}
