/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

volatile sig_atomic_t errno_signals = 0;

void errno_handler(int sig) {
    errno_signals++;
    printf("Signal %d, errno preservation assumed\n", errno_signals);

    /* Incorrectly assumes errno is preserved */
    if (errno != 0) {
        printf("errno value: %d (may be incorrect)\n", errno);
    }
}

int main() {
    printf("FAIL: Signal handler errno preservation assumption\n");

    signal(SIGTERM, errno_handler);

    printf("PID: %d\n", getpid());
    printf("Testing errno preservation in signal handlers\n");

    /* Set errno to a known value */
    errno = ENOENT;
    printf("Set errno to ENOENT (%d)\n", ENOENT);

    /* Generate signal, assuming errno is preserved */
    raise(SIGTERM);

    printf("After signal, errno: %d\n", errno);
    printf("Signal count: %d\n", errno_signals);
    printf("Code incorrectly assumes errno preservation\n");

    return 0;
}