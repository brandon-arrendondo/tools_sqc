/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/* Compliant: Using self-pipe trick to avoid shared access */
static int signal_pipe[2];  /* Pipe for signal communication */

void safe_signal_handler(int sig) {
    /* Compliant: Only write signal number to pipe, no shared object access */
    char signal_byte = (char)sig;
    ssize_t result = write(signal_pipe[1], &signal_byte, 1);
    (void)result;  /* Avoid unused variable warning */
}

int main() {
    printf("Demonstrating self-pipe trick for safe signal handling\n");
    printf("PID: %d\n", getpid());

    /* Create pipe for signal communication */
    if (pipe(signal_pipe) == -1) {
        perror("pipe");
        exit(1);
    }

    /* Make write end non-blocking */
    int flags = fcntl(signal_pipe[1], F_GETFL);
    fcntl(signal_pipe[1], F_SETFL, flags | O_NONBLOCK);

    /* Install signal handlers */
    signal(SIGUSR1, safe_signal_handler);
    signal(SIGUSR2, safe_signal_handler);
    signal(SIGTERM, safe_signal_handler);

    /* Main program variables (not accessed by signal handler) */
    int iteration_count = 0;
    char status_message[128];
    int signals_processed = 0;

    printf("Signal handlers installed. Send SIGUSR1, SIGUSR2, or SIGTERM\n");

    for (int i = 0; i < 50; i++) {
        iteration_count = i;
        sprintf(status_message, "Processing iteration %d", i);

        /* Check for signals using the self-pipe */
        fd_set readfds;
        struct timeval timeout = {0, 100000};  /* 100ms timeout */

        FD_ZERO(&readfds);
        FD_SET(signal_pipe[0], &readfds);

        int result = select(signal_pipe[0] + 1, &readfds, NULL, NULL, &timeout);

        if (result > 0 && FD_ISSET(signal_pipe[0], &readfds)) {
            char signal_byte;
            while (read(signal_pipe[0], &signal_byte, 1) > 0) {
                int received_signal = (int)signal_byte;
                signals_processed++;

                printf("Main: Safely processed signal %d (total: %d)\n",
                       received_signal, signals_processed);

                if (received_signal == SIGTERM) {
                    printf("SIGTERM received, shutting down safely\n");
                    goto cleanup;
                }
            }
        }

        printf("Main: %s (count=%d, signals=%d)\n",
               status_message, iteration_count, signals_processed);

        usleep(100000);
    }

cleanup:
    close(signal_pipe[0]);
    close(signal_pipe[1]);
    printf("Program completed safely using self-pipe trick\n");
    return 0;
}