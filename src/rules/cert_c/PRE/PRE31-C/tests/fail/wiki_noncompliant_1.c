/*
 * Rule: PRE31-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE31-C violation
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))
 
void func(int n) {
  /* Validate that n is within the desired range */
  int m = ABS(++n);

  /* ... */
}