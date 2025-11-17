/*
 * Rule: SIG34-C
 * Source: wiki
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
 
void handler(int signum) {
  if (signal(signum, handler) == SIG_ERR) {
    /* Handle error */
  }
  /* Handle signal */
}
 
void func(void) {
  if (signal(SIGUSR1, handler) == SIG_ERR) {
    /* Handle error */
  }
}