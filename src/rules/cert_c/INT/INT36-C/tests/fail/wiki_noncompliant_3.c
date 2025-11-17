/*
 * Rule: INT36-C
 * Source: wiki
 * Status: FAIL - Should trigger INT36-C violation
 */

unsigned int *g(void) {
  unsigned int *ptr = 0xdeadbeef;
  /* ... */
  return ptr;
}