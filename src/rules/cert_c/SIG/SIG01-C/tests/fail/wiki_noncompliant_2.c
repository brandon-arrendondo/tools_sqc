/*
 * Rule: SIG01-C
 * Source: wiki
 * Status: FAIL - Should trigger SIG01-C violation
 */

void handler(int signum) {
  if (signal(signum, handler) == SIG_ERR) {
    /* Handle error */
  }
  /* Handle signal */
}