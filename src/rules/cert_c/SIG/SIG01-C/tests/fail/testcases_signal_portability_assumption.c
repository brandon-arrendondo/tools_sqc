/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t portable_count = 0;

void portable_handler(int sig) {
    portable_count++;
    printf("Portable handler: %d\n", portable_count);

#ifdef __linux__
    printf("Linux-specific signal handling\n");
#elif defined(__APPLE__)
    printf("macOS-specific signal handling\n");
#elif defined(_WIN32)
    printf("Windows signal handling (limited)\n");
#else
    printf("Unknown platform signal handling\n");
#endif
}

int main() {
    printf("FAIL: Signal portability assumptions across platforms\n");

    /* Assumes signal() behaves identically across all platforms */
    if (signal(SIGTERM, portable_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Code assumes portable signal behavior\n");

    /* Platform-specific signal assumptions */
#ifdef SIGPWR
    printf("Assuming SIGPWR is available (Linux-specific)\n");
    signal(SIGPWR, portable_handler);
#endif

#ifdef SIGINFO
    printf("Assuming SIGINFO is available (BSD-specific)\n");
    signal(SIGINFO, portable_handler);
#endif

    /* Send test signal */
    raise(SIGTERM);

    sleep(1);

    printf("Portable signals: %d\n", portable_count);
    printf("Code makes incorrect portability assumptions\n");

    return 0;
}