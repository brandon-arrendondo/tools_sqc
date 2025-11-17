/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t system_events = 0;

void system_handler(int sig) {
    system_events++;
    printf("System event %d handled\n", system_events);
}

int main() {
    printf("FAIL: Assuming system-specific signal behavior\n");

    /* Assumes signal() works consistently across all UNIX variants */
    signal(SIGPIPE, system_handler);

    printf("PID: %d\n", getpid());
    printf("Code written for specific system, may fail on others\n");

#if defined(__linux__)
    printf("Compiled for Linux - assuming Linux signal semantics\n");
#elif defined(__APPLE__)
    printf("Compiled for macOS - assuming BSD signal semantics\n");
#elif defined(__sun)
    printf("Compiled for Solaris - assuming SysV signal semantics\n");
#else
    printf("Unknown system - signal behavior undefined\n");
#endif

    /* Generate multiple SIGPIPE signals */
    int i;
    for (i = 0; i < 4; i++) {
        raise(SIGPIPE);
        usleep(250000);
    }

    printf("System events handled: %d\n", system_events);
    printf("Behavior may vary across different UNIX systems\n");

    return 0;
}