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
#include <errno.h>

int signal_pipe[2];
volatile sig_atomic_t signal_received = 0;

void safe_signal_handler(int sig) {
    // Compliant: Self-pipe trick - only write to pipe in handler
    char signal_byte = (char)sig;
    ssize_t result = write(signal_pipe[1], &signal_byte, 1);

    // Mark that signal was received
    signal_received = 1;

    (void)result; // Suppress unused variable warning
}

int main() {
    struct sigaction sa;

    // Create self-pipe
    if (pipe(signal_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Make write end non-blocking
    int flags = fcntl(signal_pipe[1], F_GETFL);
    fcntl(signal_pipe[1], F_SETFL, flags | O_NONBLOCK);

    sa.sa_handler = safe_signal_handler;
    sigemptyset(&sa.sa_mask);

    // Compliant: Mask signal during handler execution
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);

    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Using self-pipe trick for safe signal handling\n");

    while (1) {
        // Check for signals via pipe
        char signal_byte;
        ssize_t bytes_read = read(signal_pipe[0], &signal_byte, 1);

        if (bytes_read > 0) {
            printf("Received signal %d via self-pipe\n", (int)signal_byte);

            // Safe to do complex operations here outside signal handler
            printf("Processing signal %d safely in main loop\n", (int)signal_byte);

            // Simulate complex work
            for (int i = 0; i < 3; i++) {
                printf("  Processing step %d\n", i + 1);
                sleep(1);
            }
        } else if (bytes_read == -1 && errno != EAGAIN) {
            perror("read");
            break;
        }

        printf("Signal flag: %d\n", signal_received);
        usleep(100000); // 100ms
    }

    close(signal_pipe[0]);
    close(signal_pipe[1]);
    return 0;
}