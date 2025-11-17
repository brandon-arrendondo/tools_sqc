/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signalfd.h>
#include <sys/select.h>
#include <errno.h>

/* Compliant: Using signalfd mechanism for safe signal access */
/* Note: signalfd is Linux-specific, but demonstrates safe signal handling */

int main() {
    printf("Demonstrating safe signal handling using signalfd mechanism\n");
    printf("PID: %d\n", getpid());

#ifdef __linux__
    /* Linux-specific signalfd implementation */
    sigset_t mask;
    int signal_fd;

    /* Block signals that we want to handle via signalfd */
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    /* Create signalfd */
    signal_fd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (signal_fd == -1) {
        perror("signalfd");
        exit(1);
    }

    printf("signalfd created successfully\n");
    printf("Send SIGUSR1, SIGUSR2, or SIGTERM to test\n");

    /* Application state - safe to access since signals are handled synchronously */
    int message_count = 0;
    int error_count = 0;
    int shutdown_requested = 0;
    char status_buffer[512];

    for (int i = 0; i < 50 && !shutdown_requested; i++) {
        fd_set readfds;
        struct timeval timeout;

        FD_ZERO(&readfds);
        FD_SET(signal_fd, &readfds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;  /* 200ms timeout */

        int result = select(signal_fd + 1, &readfds, NULL, NULL, &timeout);

        if (result > 0 && FD_ISSET(signal_fd, &readfds)) {
            struct signalfd_siginfo signal_info;
            ssize_t bytes_read = read(signal_fd, &signal_info, sizeof(signal_info));

            if (bytes_read == sizeof(signal_info)) {
                /* Safe to access any data since this is synchronous */
                int received_signal = signal_info.ssi_signo;

                switch (received_signal) {
                    case SIGUSR1:
                        message_count++;
                        snprintf(status_buffer, sizeof(status_buffer),
                                "Message signal received (count: %d)", message_count);
                        printf("SIGUSR1: %s\n", status_buffer);
                        break;

                    case SIGUSR2:
                        error_count++;
                        snprintf(status_buffer, sizeof(status_buffer),
                                "Error signal received (count: %d)", error_count);
                        printf("SIGUSR2: %s\n", status_buffer);
                        break;

                    case SIGTERM:
                        shutdown_requested = 1;
                        snprintf(status_buffer, sizeof(status_buffer),
                                "Shutdown requested via SIGTERM");
                        printf("SIGTERM: %s\n", status_buffer);
                        break;

                    default:
                        printf("Unexpected signal: %d\n", received_signal);
                        break;
                }
            }
        } else if (result == 0) {
            /* Timeout - continue with normal processing */
            snprintf(status_buffer, sizeof(status_buffer),
                    "Normal processing: iter=%d, messages=%d, errors=%d",
                    i, message_count, error_count);

            if (i % 10 == 0) {
                printf("Main: %s\n", status_buffer);
            }
        } else {
            perror("select");
            break;
        }

        /* Safe to access any application state here */
        if (message_count > 0 && message_count % 5 == 0) {
            printf("Status: %d messages processed, %d errors handled\n",
                   message_count, error_count);
        }

        usleep(50000);  /* Additional work simulation */
    }

    close(signal_fd);

    printf("Program completed safely using signalfd\n");
    printf("Final stats: messages=%d, errors=%d\n", message_count, error_count);

#else
    /* Fallback for non-Linux systems using standard signal handling */
    printf("signalfd not available on this platform\n");
    printf("Using alternative safe signal handling approach\n");

    volatile sig_atomic_t safe_flag = 0;
    volatile sig_atomic_t safe_signal_type = 0;

    /* Simple safe signal handler */
    void safe_handler(int sig) {
        safe_flag = 1;
        safe_signal_type = sig;
    }

    signal(SIGUSR1, safe_handler);
    signal(SIGUSR2, safe_handler);
    signal(SIGTERM, safe_handler);

    /* Application state */
    int message_count = 0;
    int error_count = 0;
    int shutdown_requested = 0;

    for (int i = 0; i < 50 && !shutdown_requested; i++) {
        if (safe_flag) {
            safe_flag = 0;  /* Reset flag */
            int received_signal = safe_signal_type;

            switch (received_signal) {
                case SIGUSR1:
                    message_count++;
                    printf("SIGUSR1: Message signal received (count: %d)\n", message_count);
                    break;
                case SIGUSR2:
                    error_count++;
                    printf("SIGUSR2: Error signal received (count: %d)\n", error_count);
                    break;
                case SIGTERM:
                    shutdown_requested = 1;
                    printf("SIGTERM: Shutdown requested\n");
                    break;
            }
        }

        if (i % 10 == 0) {
            printf("Main: iter=%d, messages=%d, errors=%d\n",
                   i, message_count, error_count);
        }

        usleep(100000);
    }

    printf("Program completed safely using alternative approach\n");
    printf("Final stats: messages=%d, errors=%d\n", message_count, error_count);
#endif

    return 0;
}