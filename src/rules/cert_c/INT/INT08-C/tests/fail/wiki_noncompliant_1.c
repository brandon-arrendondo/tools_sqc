/*
 * Rule: INT08-C
 * Source: wiki
 * Status: FAIL - Should trigger INT08-C violation
 */

int i = /* Expression that evaluates to the value 32767 */;
/* ... */
if (i + 1 <= i) {
  /* Handle overflow */
}
/* Expression involving i + 1 */