/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signalfd.h>
#include <stdint.h>

int main() {
    sigset_t mask;
    int sfd;
    struct signalfd_siginfo si;
    ssize_t s;
    int signal_count = 0;

    printf("SIG34-C COMPLIANT: Using signalfd for safe signal handling\n");
    printf("No signal handlers or signal() calls needed\n");
    printf("PID: %d\n", getpid());

    // SAFE: Using signalfd - no signal handlers required
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGTERM);

    // Block signals so they are delivered to signalfd instead of handlers
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    // Create signalfd to receive signals synchronously
    sfd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (sfd == -1) {
        perror("signalfd");
        exit(EXIT_FAILURE);
    }

    printf("signalfd created successfully - signals will be delivered synchronously\n");
    printf("Send SIGUSR1, SIGUSR2, or SIGTERM to test signalfd handling\n");

    // SAFE: Synchronous signal processing - no race conditions
    while (signal_count < 8) {
        s = read(sfd, &si, sizeof(si));
        if (s == sizeof(si)) {
            signal_count++;

            printf("Received signal %d synchronously via signalfd (count: %d)\n",
                   si.ssi_signo, signal_count);

            // Process signal synchronously - completely safe
            switch (si.ssi_signo) {
                case SIGUSR1:
                    printf("Processing SIGUSR1 synchronously\n");
                    break;
                case SIGUSR2:
                    printf("Processing SIGUSR2 synchronously\n");
                    break;
                case SIGTERM:
                    printf("Processing SIGTERM synchronously\n");
                    break;
                default:
                    printf("Unexpected signal %d\n", si.ssi_signo);
                    break;
            }

            if (si.ssi_signo == SIGTERM) {
                printf("SIGTERM received, shutting down safely\n");
                break;
            }
        } else {
            perror("read signalfd");
            break;
        }
    }

    close(sfd);
    printf("signalfd safe signal handling complete: %d signals processed\n", signal_count);
    return 0;
}