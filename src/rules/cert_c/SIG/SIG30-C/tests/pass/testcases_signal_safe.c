/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Only async-signal-safe operations in handlers
 */

#include <signal.h>

/* Handler that only sets volatile flag */
volatile sig_atomic_t flag = 0;
void safe_handler(int sig) {
    flag = 1;
    (void)sig;
}

/* Handler that calls _exit (async-signal-safe) */
void exit_handler(int sig) {
    _exit(sig);
}

void setup_safe(void) {
    signal(SIGINT, safe_handler);
    signal(SIGTERM, exit_handler);
}
