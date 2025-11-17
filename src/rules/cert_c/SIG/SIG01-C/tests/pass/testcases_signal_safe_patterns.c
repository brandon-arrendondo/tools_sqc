/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t safe_count = 0;
volatile sig_atomic_t error_flag = 0;

void signal_safe_handler(int sig) {
    safe_count++;

    /* Only use async-signal-safe functions */

    /* write() is async-signal-safe */
    const char msg[] = "Safe handler executed\n";
    write(STDOUT_FILENO, msg, strlen(msg));

    /* sig_atomic_t operations are safe */
    if (safe_count > 10) {
        error_flag = 1;
    }
}

/* Signal-safe logging function */
void safe_log(const char* message) {
    write(STDOUT_FILENO, message, strlen(message));
}

/* Install handler using only safe patterns */
int install_safe_handler(int sig, void (*handler)(int)) {
    struct sigaction sa;

    /* Use only safe operations for setup */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    /* Block the signal during handler execution for safety */
    sigaddset(&sa.sa_mask, sig);

    /* Use minimal flags for maximum safety */
    sa.sa_flags = 0;

#ifdef SA_RESTART
    /* SA_RESTART improves reliability of interrupted system calls */
    sa.sa_flags |= SA_RESTART;
#endif

    /* Install handler */
    return sigaction(sig, &sa, NULL);
}

/* Safely check if handler is still installed */
int verify_handler_safety(int sig, void (*expected_handler)(int)) {
    struct sigaction current;

    if (sigaction(sig, NULL, &current) == -1) {
        return -1;
    }

    /* Verify handler is what we expect */
    return (current.sa_handler == expected_handler) ? 1 : 0;
}

int main() {
    printf("PASS: Signal-safe programming patterns\n");

    printf("PID: %d\n", getpid());

    /* Install handler using safe patterns */
    if (install_safe_handler(SIGINT, signal_safe_handler) == -1) {
        perror("install_safe_handler");
        exit(EXIT_FAILURE);
    }

    safe_log("Signal-safe handler installed\n");

    /* Verify handler installation */
    if (verify_handler_safety(SIGINT, signal_safe_handler) == 1) {
        safe_log("Handler installation verified\n");
    } else {
        safe_log("Handler verification failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Press Ctrl+C to test signal-safe handler\n");

    /* Main loop using safe patterns */
    while (safe_count < 3 && !error_flag) {
        pause();

        /* Re-verify handler after each signal */
        if (verify_handler_safety(SIGINT, signal_safe_handler) != 1) {
            safe_log("Handler no longer installed safely\n");
            break;
        }
    }

    if (error_flag) {
        safe_log("Error condition detected\n");
    }

    /* Safe reporting */
    char count_msg[50];
    snprintf(count_msg, sizeof(count_msg), "Safe signals handled: %d\n", safe_count);
    safe_log(count_msg);

    safe_log("Signal-safe patterns completed successfully\n");

    return 0;
}