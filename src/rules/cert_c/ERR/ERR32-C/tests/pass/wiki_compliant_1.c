/*
 * Rule: ERR32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR32-C violation
 */

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

typedef void (*pfv)(int);

void handler(int signum) {
  pfv old_handler = signal(signum, SIG_DFL);
  if (old_handler == SIG_ERR) {
    abort();
  }
}

int main(void) {
  pfv old_handler = signal(SIGINT, handler);
  if (old_handler == SIG_ERR) {
    perror("SIGINT handler");
    /* Handle error */
  }

  /* Main code loop */

  return EXIT_SUCCESS;
}