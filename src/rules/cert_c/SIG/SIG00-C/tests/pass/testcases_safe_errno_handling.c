/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

volatile sig_atomic_t signal_count = 0;

void errno_safe_handler(int sig) {
    // Compliant: Save and restore errno to avoid corruption
    int saved_errno = errno;

    signal_count++;

    // Only async-safe operations in handler
    char msg[] = "Signal received\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    // Restore errno
    errno = saved_errno;
}

int main() {
    struct sigaction sa;

    sa.sa_handler = errno_safe_handler;
    sigemptyset(&sa.sa_mask);

    // Compliant: Mask signal during handler execution
    sigaddset(&sa.sa_mask, SIGUSR1);

    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Handler safely preserves errno\n");

    while (1) {
        // Operations that set errno
        errno = 0;

        FILE* fp = fopen("nonexistent_file.txt", "r");
        if (fp == NULL) {
            int expected_errno = errno;
            printf("Expected errno: %d (%s)\n", expected_errno, strerror(expected_errno));

            // Brief delay where signal could arrive
            usleep(100000);

            // Check that errno wasn't corrupted by signal handler
            if (errno == expected_errno) {
                printf("Good: errno preserved correctly (%d)\n", errno);
            } else {
                printf("ERROR: errno corrupted! Expected %d, got %d\n",
                       expected_errno, errno);
            }
        }

        printf("Signals received: %d\n", signal_count);
        sleep(2);
    }

    return 0;
}