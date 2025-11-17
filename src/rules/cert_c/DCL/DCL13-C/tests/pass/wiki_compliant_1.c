/*
 * Rule: DCL13-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL13-C violation
 */

void foo(const int * x) {
  if (x != NULL) {
    printf("Value is %d\n", *x);
  }
  /* ... */
}