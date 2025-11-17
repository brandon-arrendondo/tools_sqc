/*
 * Rule: INT14-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT14-C violation
 */

int compute(int x) {
  return 5 * x + 1;
}
// ...
 
int x = compute(50);