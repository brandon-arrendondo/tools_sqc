/*
 * Rule: EXP46-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP46-C violation
 */

if (getuid() == 0 && getgid() == 0) {
  /* ... */
}