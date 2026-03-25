/*
 * Rule: MSC38-C
 * Source: testcases
 * Status: PASS - Signal handler uses only async-signal-safe functions
 */

#include <signal.h>
#include <unistd.h>

/* Only uses _exit — async-signal-safe */
void safe_handler(int sig) {
    (void)sig;
    _exit(1);
}

void setup_safe(void) {
    signal(SIGINT, safe_handler);
}
