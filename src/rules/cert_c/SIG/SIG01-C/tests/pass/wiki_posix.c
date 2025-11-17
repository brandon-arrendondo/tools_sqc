/*
 * Rule: SIG01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

/* 
 * Equivalent to signal(SIGUSR1, handler) but makes
 * signal nonpersistent.
 */
struct sigaction act;
act.sa_handler = handler;
act.sa_flags = SA_RESETHAND;
if (sigemptyset(&act.sa_mask) != 0) {
  /* Handle error */
}
if (sigaction(SIGUSR1, &act, NULL) != 0) {
  /* Handle error */
}