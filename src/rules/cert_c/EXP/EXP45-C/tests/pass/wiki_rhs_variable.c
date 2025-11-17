/*
 * Rule: EXP45-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP45-C violation
 */

while ('\t' = ch || ' ' == ch || '\n' == ch) {
  /* ... */
}