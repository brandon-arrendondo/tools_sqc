/*
 * Rule: MEM10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM10-C violation
 */

void incr(int *intptr) {
  if (!valid(intptr)) {
    /* Handle error */
  }
  (*intptr)++;
}