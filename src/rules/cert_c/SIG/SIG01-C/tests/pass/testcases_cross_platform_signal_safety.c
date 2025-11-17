/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t safety_count = 0;

void cross_platform_handler(int sig) {
    safety_count++;
    /* Use only async-signal-safe functions */
    /* write() is async-signal-safe across all platforms */
    char msg[] = "Safe signal handled\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

/* Cross-platform signal installation */
int install_cross_platform_handler(int sig, void (*handler)(int)) {
    struct sigaction sa;

    /* Initialize sigaction structure for cross-platform safety */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    /* Set flags for maximum cross-platform compatibility */
    sa.sa_flags = 0;

#ifdef SA_RESTART
    /* Enable automatic restart of interrupted system calls if available */
    sa.sa_flags |= SA_RESTART;
#endif

#ifdef SA_NODEFER
    /* Prevent signal from being masked during handler execution if desired */
    /* sa.sa_flags |= SA_NODEFER; */  /* Commented out for safety */
#endif

    /* Install the handler */
    if (sigaction(sig, &sa, NULL) == -1) {
        return -1;
    }

    return 0;
}

/* Verify signal handling works across platforms */
int verify_cross_platform_behavior(int sig, void (*handler)(int)) {
    struct sigaction current;

    /* Get current signal action */
    if (sigaction(sig, NULL, &current) == -1) {
        return -1;
    }

    /* Verify our handler is installed */
    if (current.sa_handler != handler) {
        return 0;  /* Handler not installed correctly */
    }

    /* Check if sigaction semantics are working (handler should persist) */
    int initial_count = safety_count;
    raise(sig);
    usleep(100000);  /* Allow signal processing */

    raise(sig);
    usleep(100000);  /* Allow signal processing */

    /* With sigaction, both signals should be handled */
    return (safety_count >= initial_count + 2) ? 1 : 0;
}

int main() {
    printf("PASS: Cross-platform signal safety\n");

    printf("PID: %d\n", getpid());

#ifdef __linux__
    printf("Linux platform detected\n");
#elif defined(__APPLE__)
    printf("macOS platform detected\n");
#elif defined(__FreeBSD__)
    printf("FreeBSD platform detected\n");
#elif defined(_WIN32)
    printf("Windows platform detected (limited signal support)\n");
#else
    printf("Unknown platform - using POSIX-compliant approach\n");
#endif

    /* Install cross-platform safe handler */
    if (install_cross_platform_handler(SIGTERM, cross_platform_handler) == -1) {
        perror("install_cross_platform_handler");
        exit(EXIT_FAILURE);
    }

    printf("Cross-platform handler installed\n");

    /* Verify behavior works correctly */
    int verification = verify_cross_platform_behavior(SIGTERM, cross_platform_handler);
    if (verification == 1) {
        printf("Cross-platform signal behavior verified\n");
    } else if (verification == 0) {
        printf("WARNING: Unexpected signal behavior detected\n");
    } else {
        perror("Signal verification failed");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGTERM to test cross-platform safety\n");
    raise(SIGTERM);
    sleep(1);

    printf("Cross-platform safety count: %d\n", safety_count);
    printf("Signal handling verified safe across platforms\n");

    return 0;
}