/*
 * Rule: DCL17-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL17-C violation
 */

const volatile int x;
volatile int y;
void foo(void) {
  for(y = 0; y < 10; y++) {
    int z = x;
  }
}