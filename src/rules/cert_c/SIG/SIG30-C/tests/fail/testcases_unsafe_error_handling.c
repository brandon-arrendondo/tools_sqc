/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

void error_handler(int sig) {
    // VIOLATION: perror() is not async-safe
    errno = EINVAL;  // Setting errno is generally safe
    perror("Signal handler error");

    // VIOLATION: strerror() is not async-safe
    char *error_msg = strerror(errno);

    // VIOLATION: fprintf to stderr is not async-safe
    fprintf(stderr, "Error in signal handler: %s\n", error_msg);

    // VIOLATION: ferror() and clearerr() are not async-safe
    if (ferror(stderr)) {
        clearerr(stderr);
    }

    // VIOLATION: dprintf() is not async-safe on all systems
    dprintf(STDERR_FILENO, "Signal %d caused error\n", sig);

    // VIOLATION: exit() is not async-safe (should use _exit)
    if (sig == SIGTERM) {
        exit(1);  // Should use _exit(1) instead
    }
}

int main() {
    printf("Demonstrating unsafe error handling in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, error_handler);
    signal(SIGTERM, error_handler);

    printf("Send SIGUSR1 to trigger unsafe error handling\n");

    while (1) {
        pause();
    }

    return 0;
}