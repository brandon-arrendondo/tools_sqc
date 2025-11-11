/*
 * Rule: INT14-C
 * Source: wiki
 * Status: FAIL - Should trigger INT14-C violation
 */

int compute(int x) {
  int y = x << 2;
  x += y + 1;
  return x;
}
// ...
 
int x = compute(50);