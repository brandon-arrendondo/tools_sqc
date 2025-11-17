/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signalfd.h>
#include <errno.h>

int main() {
    printf("Demonstrating signalfd for safe signal handling (Linux-specific)\n");
    printf("PID: %d\n", getpid());

    sigset_t mask;
    int sfd;
    struct signalfd_siginfo si;

    // Block signals for all threads
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    // Create signalfd to receive signals synchronously
    sfd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (sfd == -1) {
        perror("signalfd");
        printf("signalfd not supported on this system - using fallback\n");

        // Fallback: simple signal handler
        volatile sig_atomic_t received = 0;

        void fallback_handler(int sig) {
            received = sig;
        }

        signal(SIGUSR1, fallback_handler);
        signal(SIGUSR2, fallback_handler);
        signal(SIGTERM, fallback_handler);

        // Unblock signals for fallback
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        printf("Using fallback signal handling\n");
        printf("Send SIGUSR1, SIGUSR2, or SIGTERM\n");

        while (1) {
            if (received) {
                printf("Received signal %d safely via fallback handler\n", (int)received);
                if (received == SIGTERM) {
                    printf("Terminating...\n");
                    break;
                }
                received = 0;
            }
            usleep(100000);
        }

        return 0;
    }

    printf("Using signalfd for synchronous signal handling\n");
    printf("No signal handlers needed - completely safe!\n");
    printf("Send SIGUSR1, SIGUSR2, or SIGTERM\n");

    while (1) {
        ssize_t s = read(sfd, &si, sizeof(si));

        if (s == sizeof(si)) {
            // Process signal safely in normal execution context
            printf("Received signal %d via signalfd\n", si.ssi_signo);

            switch (si.ssi_signo) {
                case SIGUSR1:
                    printf("Processing SIGUSR1 safely in main context\n");
                    break;
                case SIGUSR2:
                    printf("Processing SIGUSR2 safely in main context\n");
                    break;
                case SIGTERM:
                    printf("Terminating safely...\n");
                    close(sfd);
                    exit(0);
                default:
                    printf("Unexpected signal %d\n", si.ssi_signo);
                    break;
            }
        } else if (s == -1 && errno != EINTR) {
            perror("read signalfd");
            break;
        }
    }

    close(sfd);
    return 0;
}