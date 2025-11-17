/*
 * Rule: INT14-C
 * Source: wiki
 * Status: FAIL - Should trigger INT14-C violation
 */

int compute(int x) {
  x >>= 2;
  return x;
}
// ...
 
int x = compute(-50);