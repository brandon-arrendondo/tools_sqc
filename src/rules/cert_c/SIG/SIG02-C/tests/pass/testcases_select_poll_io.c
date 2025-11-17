/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

// Signal handler only for critical system events
void critical_handler(int sig) {
    if (sig == SIGTERM) {
        printf("CRITICAL: System termination signal - performing clean shutdown\n");
        exit(0);
    }
}

int main() {
    printf("Using select/poll for I/O events, signals only for critical system events (GOOD)\n");

    // Set up signal handler only for critical events
    signal(SIGTERM, critical_handler);

    int pipefd1[2], pipefd2[2];
    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t child1 = fork();
    if (child1 == 0) {
        // Child 1 - data producer
        close(pipefd1[0]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        printf("Producer 1: Starting data generation\n");

        sleep(2);
        printf("Producer 1: Sending data packet 1\n");
        write(pipefd1[1], "DATA_PACKET_1", 14);

        sleep(3);
        printf("Producer 1: Sending data packet 2\n");
        write(pipefd1[1], "DATA_PACKET_2", 14);

        close(pipefd1[1]);
        exit(0);
    }

    pid_t child2 = fork();
    if (child2 == 0) {
        // Child 2 - event producer
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);

        printf("Producer 2: Starting event generation\n");

        sleep(4);
        printf("Producer 2: Sending event notification\n");
        write(pipefd2[1], "EVENT_TRIGGER", 14);

        sleep(2);
        printf("Producer 2: Sending completion event\n");
        write(pipefd2[1], "TASK_COMPLETE", 14);

        close(pipefd2[1]);
        exit(0);
    }

    // Parent process - I/O multiplexer
    close(pipefd1[1]);
    close(pipefd2[1]);

    printf("I/O Multiplexer: Starting select-based event loop\n");

    fd_set readfds;
    int max_fd = (pipefd1[0] > pipefd2[0]) ? pipefd1[0] : pipefd2[0];
    int events_processed = 0;
    char buffer[256];

    while (events_processed < 4) {
        FD_ZERO(&readfds);
        FD_SET(pipefd1[0], &readfds);
        FD_SET(pipefd2[0], &readfds);

        printf("I/O Multiplexer: Waiting for I/O events using select()\n");

        int ready = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ready == -1) {
            if (errno == EINTR) {
                printf("I/O Multiplexer: Select interrupted by signal, continuing\n");
                continue;
            }
            perror("select");
            break;
        }

        if (ready > 0) {
            // Check data pipe
            if (FD_ISSET(pipefd1[0], &readfds)) {
                ssize_t bytes_read = read(pipefd1[0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("I/O Multiplexer: Received data: %s\n", buffer);
                    printf("I/O Multiplexer: Processing data using normal business logic\n");
                    events_processed++;
                } else if (bytes_read == 0) {
                    printf("I/O Multiplexer: Data pipe closed\n");
                    FD_CLR(pipefd1[0], &readfds);
                }
            }

            // Check event pipe
            if (FD_ISSET(pipefd2[0], &readfds)) {
                ssize_t bytes_read = read(pipefd2[0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("I/O Multiplexer: Received event: %s\n", buffer);
                    printf("I/O Multiplexer: Handling event with appropriate response\n");
                    events_processed++;
                } else if (bytes_read == 0) {
                    printf("I/O Multiplexer: Event pipe closed\n");
                    FD_CLR(pipefd2[0], &readfds);
                }
            }
        }
    }

    close(pipefd1[0]);
    close(pipefd2[0]);

    wait(NULL);
    wait(NULL);

    printf("I/O event processing completed using proper select() mechanism\n");

    return 0;
}