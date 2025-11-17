/*
 * Rule: EXP30-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP30-C violation
 */

extern void func(int i, int j);
 
void f(int i) {
  func(i++, i);
}