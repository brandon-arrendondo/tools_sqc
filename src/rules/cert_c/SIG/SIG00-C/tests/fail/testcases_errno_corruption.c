/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

volatile sig_atomic_t signal_received = 0;

void errno_handler(int sig) {
    signal_received++;

    // Violation: Modifying errno without saving/restoring
    // and without proper signal masking
    errno = EINTR;

    printf("Handler: Set errno to EINTR\n");

    // Call function that may change errno
    char* invalid_file = "nonexistent_file_12345.txt";
    FILE* fp = fopen(invalid_file, "r");
    if (fp == NULL) {
        printf("Handler: fopen failed, errno = %d (%s)\n",
               errno, strerror(errno));
    }

    // Delay to increase chance of interruption
    sleep(1);

    errno = 0; // Reset errno
    printf("Handler: Reset errno to 0\n");
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = errno_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: errno can be corrupted by signal interruption
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 to corrupt errno handling\n");

    while (1) {
        // Main thread operations that depend on errno
        errno = 0;

        char* test_file = "another_nonexistent_file.txt";
        FILE* fp = fopen(test_file, "r");

        if (fp == NULL) {
            printf("Main: fopen failed, errno = %d (%s)\n",
                   errno, strerror(errno));

            // Check if errno was corrupted
            if (errno != ENOENT && errno != 0) {
                printf("Main: ERROR - errno was corrupted! Expected ENOENT(2), got %d\n",
                       errno);
            }
        }

        printf("Main: Signals received: %d\n", signal_received);
        sleep(2);
    }

    return 0;
}