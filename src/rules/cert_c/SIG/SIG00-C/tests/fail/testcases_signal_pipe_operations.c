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
#include <errno.h>

int pipe_fd[2];
volatile sig_atomic_t message_count = 0;

void pipe_handler(int sig) {
    message_count++;
    char message[256];

    snprintf(message, sizeof(message), "Signal %d message %d\n",
             sig, message_count);

    printf("Handler: Writing to pipe: %s", message);

    // Violation: Pipe operations without proper signal masking
    // can cause partial writes and data corruption
    ssize_t written = 0;
    ssize_t total = strlen(message);

    while (written < total) {
        ssize_t result = write(pipe_fd[1], message + written, total - written);

        if (result == -1) {
            if (errno == EINTR) {
                printf("Handler: Write interrupted\n");
                continue;
            }
            perror("Handler: write failed");
            return;
        }

        written += result;

        // Create vulnerability window
        usleep(10000);
    }

    printf("Handler: Pipe write complete\n");
}

int main() {
    struct sigaction sa;
    char buffer[256];

    // Create pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Make pipe non-blocking for reading
    fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);

    // Install handler without masking
    sa.sa_handler = pipe_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Pipe operations can be interrupted and corrupted
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to write to pipe from handler\n");

    while (1) {
        // Read from pipe in main thread
        ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Main: Read from pipe: %s", buffer);

            // Check for corruption (incomplete messages)
            if (buffer[bytes_read - 1] != '\n') {
                printf("Main: WARNING - Incomplete message detected!\n");
            }
        } else if (bytes_read == -1 && errno != EAGAIN) {
            perror("Main: read failed");
        }

        printf("Main: Messages received: %d\n", message_count);
        sleep(1);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    return 0;
}