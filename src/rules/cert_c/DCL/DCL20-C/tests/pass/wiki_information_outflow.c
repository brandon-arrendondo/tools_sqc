/*
 * Rule: DCL20-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL20-C violation
 */

void foo(void) {
  int i = 3;
  printf("i value: %d\n", i);
}