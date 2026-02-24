/*
 * Rule: INT08-C
 * Source: wiki
 * Status: FAIL - Should trigger INT08-C violation
 *
 * Arithmetic on narrow integer type (short) without proper
 * overflow protection. The check "i + 1 <= i" is NOT a proper
 * check because it uses the overflowing expression itself.
 */

void foo(void) {
  short i = 32767;
  if (i + 1 <= i) {
    /* Handle overflow — but this check is undefined behavior */
  }
}
