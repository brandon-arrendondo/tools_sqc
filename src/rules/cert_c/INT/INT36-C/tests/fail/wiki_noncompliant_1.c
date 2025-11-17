/*
 * Rule: INT36-C
 * Source: wiki
 * Status: FAIL - Should trigger INT36-C violation
 */

void f(void) {
  char *ptr;
  /* ... */
  unsigned int number = (unsigned int)ptr;
  /* ... */
}