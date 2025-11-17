/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void emergency_exit_handler(int sig) {
    // SAFE: _exit() is async-safe (unlike exit())
    // No cleanup, no atexit handlers, just immediate termination
    _exit(128 + sig);  // Standard exit code for signal termination
}

void abort_handler(int sig) {
    // SAFE: abort() is async-safe
    abort();
}

void graceful_handler(int sig) {
    const char msg[] = "Exiting...\n";

    // SAFE: write() is async-safe
    write(STDERR_FILENO, msg, sizeof(msg) - 1);

    // SAFE: _exit() is async-safe
    _exit(0);
}

int main() {
    printf("Demonstrating safe immediate exit signal handlers\n");
    printf("PID: %d\n", getpid());

    // Different safe exit strategies
    signal(SIGTERM, emergency_exit_handler);  // Immediate exit with signal code
    signal(SIGINT, graceful_handler);         // Brief message then exit
    signal(SIGQUIT, abort_handler);           // Immediate abort

    printf("Signal handlers use only async-safe exit functions:\n");
    printf("- SIGTERM: immediate _exit() with signal code\n");
    printf("- SIGINT:  write message then _exit()\n");
    printf("- SIGQUIT: immediate abort()\n");
    printf("\nSend signals to test safe exit handlers\n");

    // Infinite loop - program will only exit via signal handlers
    while (1) {
        printf("Running... (PID: %d)\n", getpid());
        sleep(3);
    }

    // This line should never be reached
    return 0;
}