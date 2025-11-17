/*
 * Rule: EXP39-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP39-C violation
 */

enum { ROWS = 10, COLS = 15 };
 
void func(void) {
  int a[ROWS][COLS];
  int (*b)[ROWS] = a;
}