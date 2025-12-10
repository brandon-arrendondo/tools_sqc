/*
 * Rule: SIG01-C
 * Source: wiki
 * Status: FAIL - Should trigger SIG01-C violation
 *
 * This demonstrates non-compliant use of signal() on UNIX systems.
 * The signal() function has implementation-defined behavior - even
 * on UNIX, different systems may reset the handler after delivery.
 */

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int signum) {
  /* Handle signal */
}

int main(void) {
  /* VIOLATION: signal() has implementation-defined handler persistence */
  /* On some UNIX systems, handler resets to SIG_DFL after delivery */
  signal(SIGTERM, handler);

  /* Wait for signals */
  pause();

  return 0;
}
