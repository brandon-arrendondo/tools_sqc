/*
 * Rule: INT34-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT34-C violation
 */

void func(unsigned int ui_a, unsigned int ui_b) {
  unsigned int uresult = ui_a >> ui_b;
  /* ... */
}