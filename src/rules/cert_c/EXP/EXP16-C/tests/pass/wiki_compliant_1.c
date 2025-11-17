/*
 * Rule: EXP16-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP16-C violation
 */

/* First the options that are allowed only for root */
if (getuid() == 0 || geteuid() != 0) {
  /* ... */
}