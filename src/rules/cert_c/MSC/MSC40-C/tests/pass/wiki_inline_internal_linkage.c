/*
 * Rule: MSC40-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC40-C violation
 */

int I = 12;
extern inline void func(int a) {
  int b = a * I;
  /* ... */
}