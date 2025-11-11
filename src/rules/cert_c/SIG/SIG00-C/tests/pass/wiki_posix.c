/*
 * Rule: SIG00-C
 * Source: wiki
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>

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
  struct sigaction act;
  act.sa_handler = &handler;
  act.sa_flags = 0;
  if (sigemptyset(&act.sa_mask) != 0) {
    /* Handle error */
  }
  if (sigaddset(&act.sa_mask, SIGUSR1)) {
    /* Handle error */
  }
  if (sigaddset(&act.sa_mask, SIGUSR2)) {
    /* Handle error */
  }

  if (sigaction(SIGUSR1, &act, NULL) != 0) {
    /* Handle error */
  }
  if (sigaction(SIGUSR2, &act, NULL) != 0) {
    /* Handle error */
  }

  while (sig2 == 0) {
    /* Do nothing or give up CPU for a while */
  }

  /* ... */

  return 0;
}