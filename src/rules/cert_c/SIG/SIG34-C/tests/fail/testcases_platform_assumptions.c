/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t platform_calls = 0;

void platform_assuming_handler(int sig) {
    platform_calls++;
    printf("Signal %d received, making platform-specific assumptions (call %d)\n", sig, platform_calls);

    // VIOLATION: Platform-specific signal() behavior assumptions
#ifdef __linux__
    printf("Assuming Linux: signal() should be persistent\n");
    // Wrong assumption - signal() behavior varies
    if (signal(sig, platform_assuming_handler) == SIG_ERR) {
        printf("Failed to re-register on Linux\n");
    }
#elif defined(__APPLE__)
    printf("Assuming macOS: signal() resets to default\n");
    // This creates different race conditions on different platforms
    if (signal(sig, platform_assuming_handler) == SIG_ERR) {
        printf("Failed to re-register on macOS\n");
    }
#else
    printf("Unknown platform: guessing signal() behavior\n");
    // Dangerous assumption about signal() semantics
    if (signal(sig, platform_assuming_handler) == SIG_ERR) {
        printf("Failed to re-register on unknown platform\n");
    }
#endif

    // Additional platform-specific assumptions
    if (platform_calls % 2 == 0) {
        printf("Assuming signal() atomicity (wrong on many platforms)\n");
        // This assumption can lead to race conditions
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            printf("Failed to ignore SIGPIPE\n");
        }
    }

    printf("Platform-specific signal() operations complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Platform-specific signal() assumptions in handler\n");
    printf("Handler assumes different signal() behavior based on platform\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, platform_assuming_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to see platform-specific signal() assumptions\n");

    while (platform_calls < 8) {
        pause();
    }

    printf("Platform-specific calls completed: %d\n", platform_calls);
    return 0;
}