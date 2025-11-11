/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <atexit.h>

volatile sig_atomic_t cleanup_started = 0;

void cleanup_function() {
    printf("Cleanup: atexit function called\n");

    if (cleanup_started) {
        printf("Cleanup: ERROR - Cleanup already in progress!\n");
        _exit(1);
    }

    cleanup_started = 1;

    // Simulate cleanup work
    for (int i = 0; i < 5; i++) {
        printf("Cleanup: Step %d\n", i + 1);
        sleep(1);
    }

    printf("Cleanup: Complete\n");
}

void exit_handler(int sig) {
    printf("Handler: Signal %d received, calling exit()\n", sig);

    // Violation: Calling exit() from signal handler without masking
    // can cause atexit functions to be called reentrantly
    if (cleanup_started) {
        printf("Handler: Cleanup in progress, forcing immediate exit\n");
        _exit(2);
    }

    printf("Handler: Calling exit()...\n");
    exit(sig);
}

int main() {
    struct sigaction sa;

    // Register cleanup function
    if (atexit(cleanup_function) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    // Install handler without masking
    sa.sa_handler = exit_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: exit() from signal handler can corrupt cleanup
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to trigger exit from handler\n");
    printf("Multiple signals during cleanup will show the problem\n");

    while (1) {
        printf("Main: Working...\n");
        sleep(2);

        // Simulate normal exit possibility
        static int counter = 0;
        counter++;
        if (counter > 20) {
            printf("Main: Normal exit after 40 seconds\n");
            exit(0);
        }
    }

    return 0;
}