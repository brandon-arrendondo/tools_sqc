/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

volatile int log_fd = -1;
volatile sig_atomic_t signal_count = 0;

void file_handler(int sig) {
    signal_count++;
    char buffer[256];

    // Violation: File operations without proper signal masking
    // Can cause file descriptor corruption and data races
    if (log_fd == -1) {
        log_fd = open("/tmp/signal_log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (log_fd == -1) {
            perror("open in handler");
            return;
        }
    }

    snprintf(buffer, sizeof(buffer), "Signal %d received (count: %d)\n",
             sig, signal_count);

    // Vulnerable: write() can be interrupted
    write(log_fd, buffer, strlen(buffer));

    // Delay increases interruption chance
    sleep(1);

    write(log_fd, "Handler complete\n", 17);
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = file_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: File operations can be interrupted and corrupted
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Logging to /tmp/signal_log.txt\n");
    printf("Send signals rapidly to corrupt file operations\n");

    while (1) {
        // Main thread also writes to demonstrate race
        if (log_fd != -1) {
            write(log_fd, "Main thread write\n", 18);
        }
        sleep(2);
    }

    if (log_fd != -1) {
        close(log_fd);
    }

    return 0;
}