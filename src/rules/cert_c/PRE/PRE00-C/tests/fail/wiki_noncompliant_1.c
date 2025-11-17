/*
 * Rule: PRE00-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE00-C violation
 */

#define CUBE(X) ((X) * (X) * (X))
 
void func(void) {
  int i = 2;
  int a = 81 / CUBE(++i);
  /* ... */
}