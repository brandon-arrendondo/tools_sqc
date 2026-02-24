/*
 * Rule: EXP15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP15-C violation
 */

void foo(int a, int b) {
  if (a == b) {
    /* Correctly controlled by if */
    a = b + 1;
  }
}
