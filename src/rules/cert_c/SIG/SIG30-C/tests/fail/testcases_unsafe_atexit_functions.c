/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void cleanup_function(void) {
    printf("Cleanup function called\n");
}

void another_cleanup(void) {
    printf("Another cleanup function\n");
}

void atexit_handler(int sig) {
    // VIOLATION: atexit() is not async-safe
    atexit(cleanup_function);

    // VIOLATION: at_quick_exit() is not async-safe (C11)
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L
    at_quick_exit(another_cleanup);
#endif
#endif

    // VIOLATION: Registering multiple exit handlers
    atexit(another_cleanup);

    // VIOLATION: abort() triggers atexit handlers and may not be fully async-safe
    if (sig == SIGTERM) {
        abort();  // This is actually async-safe, but triggers non-async-safe atexit handlers
    }

    // VIOLATION: exit() is not async-safe and triggers atexit handlers
    if (sig == SIGUSR2) {
        exit(0);  // Should use _exit() instead
    }

    // VIOLATION: quick_exit() is not async-safe (C11)
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L
    if (sig == SIGINT) {
        quick_exit(0);
    }
#endif
#endif

    printf("Exit handler registration in signal handler - UNSAFE!\n");
}

int main() {
    printf("Demonstrating unsafe atexit functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, atexit_handler);
    signal(SIGUSR2, atexit_handler);
    signal(SIGTERM, atexit_handler);
    signal(SIGINT, atexit_handler);

    printf("Send SIGUSR1, SIGUSR2, SIGTERM, or SIGINT to trigger unsafe atexit operations\n");

    while (1) {
        pause();
    }

    return 0;
}