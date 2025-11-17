/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

volatile sig_atomic_t signal_received = 0;

void errno_safe_handler(int sig) {
    // SAFE: Save errno at start of handler
    int saved_errno = errno;

    // SAFE: Only async-safe operations
    signal_received = 1;

    // SAFE: write() is async-safe
    const char msg[] = "Signal handled safely\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);

    // SAFE: Restore errno before returning
    errno = saved_errno;
}

int main() {
    printf("Demonstrating errno preservation in signal handlers\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, errno_safe_handler);

    printf("Signal handler preserves errno correctly\n");
    printf("Send SIGUSR1 to test errno-safe signal handling\n");

    while (1) {
        // Simulate some operation that might set errno
        if (access("/nonexistent/file", F_OK) == -1) {
            int current_errno = errno;
            printf("Main: errno = %d (ENOENT expected)\n", current_errno);

            if (signal_received) {
                printf("Signal was received, checking errno preservation...\n");
                if (errno == current_errno) {
                    printf("SUCCESS: errno correctly preserved across signal handler\n");
                } else {
                    printf("ERROR: errno was modified by signal handler\n");
                }
                signal_received = 0;
            }
        }

        sleep(1);
    }

    return 0;
}