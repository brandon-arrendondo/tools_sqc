/*
 * Rule: SIG01-C
 * Source: wiki
 * Status: FAIL - Should trigger SIG01-C violation
 *
 * This demonstrates non-compliant use of signal() function.
 * The signal() function has implementation-defined behavior regarding
 * handler persistence - on some systems handlers persist, on others they reset.
 */

#include <signal.h>
#include <stdlib.h>

void handler(int signum) {
  /* Handle signal */
}

int main(void) {
  /* VIOLATION: signal() has implementation-defined handler persistence */
  signal(SIGINT, handler);

  /* Wait for signals */
  while (1) {
    /* ... */
  }

  return 0;
}
