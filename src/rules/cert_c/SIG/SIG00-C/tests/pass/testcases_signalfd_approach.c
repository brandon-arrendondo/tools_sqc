/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signalfd.h>
#include <errno.h>

int main() {
    sigset_t mask;
    int signal_fd;

    printf("PID: %d\n", getpid());
    printf("Using signalfd for safe signal handling\n");

    // Compliant: Block signals for all threads
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    // Create signalfd to receive signals synchronously
    signal_fd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (signal_fd == -1) {
        perror("signalfd");
        exit(EXIT_FAILURE);
    }

    printf("Signals blocked, using signalfd for synchronous handling\n");

    while (1) {
        struct signalfd_siginfo signal_info;
        ssize_t bytes_read = read(signal_fd, &signal_info, sizeof(signal_info));

        if (bytes_read == sizeof(signal_info)) {
            printf("Received signal %d synchronously\n", signal_info.ssi_signo);

            // Safe: This runs in normal program context, not signal handler
            printf("Signal data: PID=%d, UID=%d\n",
                   signal_info.ssi_pid, signal_info.ssi_uid);

            // Complex operations are safe here
            for (int i = 0; i < 3; i++) {
                printf("  Safe processing step %d\n", i + 1);
                sleep(1);
            }

            if (signal_info.ssi_signo == SIGTERM) {
                printf("Received SIGTERM, exiting gracefully\n");
                break;
            }
        } else if (bytes_read == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("read signalfd");
            break;
        }
    }

    close(signal_fd);
    return 0;
}