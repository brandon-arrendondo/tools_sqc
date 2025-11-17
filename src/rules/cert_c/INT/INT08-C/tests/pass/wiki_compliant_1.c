/*
 * Rule: INT08-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT08-C violation
 */

long i = /* Expression that evaluates to the value 32767 */;
/* ... */
/* No test is necessary; i is known not to overflow */
/* Expression involving i + 1 */