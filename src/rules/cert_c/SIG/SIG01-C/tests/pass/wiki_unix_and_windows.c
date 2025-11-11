/*
 * Rule: SIG01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

void handler(int signum) {
#ifndef WINDOWS
  if (signal(signum, SIG_DFL) == SIG_ERR) {
    /* Handler error */
  }
#endif
  /* Handle signal */
}