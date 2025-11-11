/*
 * Rule: INT30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT30-C violation
 */

void func(unsigned int ui_a, unsigned int ui_b) {
  unsigned int udiff;
  if (ui_a < ui_b){
    /* Handle error */
  } else {
    udiff = ui_a - ui_b;
  }
  /* ... */
}