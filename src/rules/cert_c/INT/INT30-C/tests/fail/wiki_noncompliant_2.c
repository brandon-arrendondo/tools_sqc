/*
 * Rule: INT30-C
 * Source: wiki
 * Status: FAIL - Should trigger INT30-C violation
 */

void func(unsigned int ui_a, unsigned int ui_b) {
  unsigned int udiff = ui_a - ui_b;
  /* ... */
}