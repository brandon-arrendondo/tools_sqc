/*
 * Rule: EXP16-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP16-C violation
 */

/* First the options that are allowed only for root */
if (getuid == 0 || geteuid != 0) {
  /* ... */
}