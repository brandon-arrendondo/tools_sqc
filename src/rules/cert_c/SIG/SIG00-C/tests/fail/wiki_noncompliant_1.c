/*
 * Rule: SIG00-C
 * Source: wiki
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>

volatile sig_atomic_t sig1 = 0;
volatile sig_atomic_t sig2 = 0;

void handler(int signum) {
  if (signum == SIGUSR1) {
    sig1 = 1;
  }
  else if (sig1) {
    sig2 = 1;
  }
}

int main(void) {
  if (signal(SIGUSR1, handler) == SIG_ERR) {
    /* Handle error */
  }
  if (signal(SIGUSR2, handler) == SIG_ERR) {
    /* Handler error */
  }

  while (sig2 == 0) {
    /* Do nothing or give up CPU for a while */
  }

  /* ... */

  return 0;
}