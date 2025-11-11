/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int log_fd = -1;
volatile sig_atomic_t log_sequence = 0;

void safe_logging_handler(int sig) {
    // Compliant: Only async-safe functions in signal handler
    log_sequence++;

    if (log_fd != -1) {
        // write() is async-safe
        char log_entry[] = "SIGNAL_LOG_ENTRY\n";
        write(log_fd, log_entry, sizeof(log_entry) - 1);
    }
}

int main() {
    struct sigaction sa;

    // Open log file
    log_fd = open("/tmp/safe_signal_log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("open log file");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = safe_logging_handler;
    sigemptyset(&sa.sa_mask);

    // Compliant: Mask signals to prevent handler interruption
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);

    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        close(log_fd);
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction SIGUSR2");
        close(log_fd);
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Logging to /tmp/safe_signal_log.txt\n");
    printf("Signal handlers use only async-safe operations\n");

    while (1) {
        printf("Log sequence: %d\n", log_sequence);

        // Safe to do complex logging in main thread
        char detailed_log[256];
        time_t current_time = time(NULL);
        snprintf(detailed_log, sizeof(detailed_log),
                 "MAIN_LOG: timestamp=%ld, sequence=%d\n",
                 current_time, log_sequence);

        // Complex logging is safe in main thread
        if (log_fd != -1) {
            write(log_fd, detailed_log, strlen(detailed_log));
            fsync(log_fd); // Force write to disk
        }

        printf("Main thread logged detailed entry\n");
        sleep(3);
    }

    if (log_fd != -1) {
        close(log_fd);
    }

    return 0;
}