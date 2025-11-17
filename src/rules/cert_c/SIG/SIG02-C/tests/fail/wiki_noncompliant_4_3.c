/*
 * Rule: SIG02-C
 * Source: wiki
 * Status: FAIL - Should trigger SIG02-C violation
 */

volatile sig_atomic_t xfer_aborted = 0;

static void myoob(signo) {
  /* ... */
  if (strcmp(cp, "ABOR\r\n") == 0) {
    xfer_aborted = 1;
  }
  /* ... */
}