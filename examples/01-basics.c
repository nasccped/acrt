#include <acrt.h>

// Simple integer adding function.
int add(int a, int b) {
  return a + b;
}

/* Unit test can be defined as 'ACRT_CASE(<TEST NAME>) { <TEST BODY> }'.
 *
 * The ACRT_CASE is just an macro which expansion results into:
 *
 * __attribute__((used, section(<ACRT CASES SECTION>)))
 * void <TEST NAME>(<PRIVATE PARAMS>) {
 *   ...
 * }
 *
 * It means that the <TEST NAME> must be a valid function identifier. */
ACRT_CASE(empty_case) {
  // test goes here...
}
