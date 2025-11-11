/*
 * Rule: STR09-C
 * Source: wiki
 * Status: FAIL - Should trigger STR09-C violation
 */

char ch = 'b';
if ((ch >= 'a') && (ch <= 'c')) {
  /* ... */
}