/*
 * Rule: STR09-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR09-C violation
 */

char ch = 't';
if ((ch == 'a') || (ch == 'b') || (ch == 'c')) {
  /* ... */
}