/*
 * Rule: EXP30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP30-C violation
 */

extern void func(int i, int j);
 
void f(int i) {
  i++;
  func(i, i);
}