/*
 * Rule: PRE00-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE00-C violation
 */

#define SQUARE(X) ((X) * (X))

void func(void) {
  int i = 2;
  // VIOLATION: Function-like macro used instead of inline function
  int a = SQUARE(++i);
}