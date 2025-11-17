/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

static int pipe_fd[2]; // Self-pipe for signal notification
volatile sig_atomic_t pipe_signals = 0;

void self_pipe_handler(int sig) {
    char signal_byte = (char)sig;

    // SAFE: Handler only writes to self-pipe, no signal() calls
    // This is the classic self-pipe trick for safe signal handling
    if (write(pipe_fd[1], &signal_byte, 1) == -1) {
        // Write failed, but we can't do much in signal handler
        // This is still safer than using signal() calls
    }

    pipe_signals++;
}

int main() {
    struct sigaction sa;
    char signal_byte;
    int flags;
    printf("SIG34-C COMPLIANT: Self-pipe trick implementation\n");
    printf("Safe signal handling without signal() calls in handlers\n");
    printf("PID: %d\n", getpid());

    // SAFE: Create self-pipe for signal notification
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Make write end non-blocking to prevent handler from blocking
    flags = fcntl(pipe_fd[1], F_GETFL);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }

    if (fcntl(pipe_fd[1], F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }

    // SAFE: Register handler that only writes to pipe
    sa.sa_handler = self_pipe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("Self-pipe trick setup complete:\n");
    printf("- Handlers only write to pipe\n");
    printf("- Main loop reads from pipe synchronously\n");
    printf("- No signal() calls in handlers\n");
    printf("Send SIGUSR1 and SIGUSR2 to test self-pipe trick\n");

    // SAFE: Main loop processes signals synchronously via self-pipe
    while (pipe_signals < 8) {
        if (read(pipe_fd[0], &signal_byte, 1) == 1) {
            printf("Self-pipe received signal %d synchronously (total: %d)\n",
                   (int)signal_byte, pipe_signals);

            // Process signal safely in main thread
            switch (signal_byte) {
                case SIGUSR1:
                    printf("Processing SIGUSR1 safely in main thread\n");
                    break;
                case SIGUSR2:
                    printf("Processing SIGUSR2 safely in main thread\n");
                    break;
                default:
                    printf("Unexpected signal %d via self-pipe\n", (int)signal_byte);
                    break;
            }
        } else {
            if (errno == EINTR) {
                // Interrupted by signal, continue
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available, brief pause
                usleep(10000); // 10ms
            } else {
                perror("read self-pipe");
                break;
            }
        }
    }

    // Cleanup
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    printf("Self-pipe trick demonstration complete: %d signals processed\n", pipe_signals);
    return 0;
}