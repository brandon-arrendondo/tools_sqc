/*
 * Rule: MSC40-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC40-C violation
 */

extern inline void func(void) {
  int I = 12;
  /* Perform calculations which may modify I */
}