/*
 * Rule: INT36-C
 * Source: wiki
 * Status: FAIL - Should trigger INT36-C violation
 */

void func(unsigned int flag) {
  char *ptr;
  /* ... */
  unsigned int number = (unsigned int)ptr;
  number = (number & 0x7fffff) | (flag << 23);
  ptr = (char *)number;
}