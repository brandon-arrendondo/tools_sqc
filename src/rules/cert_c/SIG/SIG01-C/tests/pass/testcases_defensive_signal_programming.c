/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t defensive_count = 0;

void defensive_handler(int sig) {
    defensive_count++;
    /* Only use async-signal-safe functions */
    write(STDOUT_FILENO, "Signal received\n", 16);
}

int reinstall_handler_if_needed(int sig, void (*handler)(int)) {
    struct sigaction sa, current_sa;

    /* Check current handler */
    if (sigaction(sig, NULL, &current_sa) == -1) {
        return -1;
    }

    /* If handler is not what we expect, reinstall it */
    if (current_sa.sa_handler != handler) {
        sa.sa_handler = handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if (sigaction(sig, &sa, NULL) == -1) {
            return -1;
        }

        write(STDOUT_FILENO, "Handler reinstalled\n", 20);
        return 1;  /* Handler was reinstalled */
    }

    return 0;  /* Handler was already correct */
}

int main() {
    struct sigaction sa;
    printf("PASS: Defensive signal programming with handler verification\n");

    /* Initial handler setup */
    sa.sa_handler = defensive_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d - Press Ctrl+C multiple times\n", getpid());
    printf("Handler uses defensive programming practices\n");

    int signals_processed = 0;
    while (signals_processed < 3) {
        pause();

        /* After each signal, verify handler is still installed */
        int result = reinstall_handler_if_needed(SIGINT, defensive_handler);
        if (result == -1) {
            perror("Handler verification failed");
            break;
        } else if (result == 1) {
            printf("Handler was reinstalled defensively\n");
        }

        signals_processed++;
        printf("Processed %d signals\n", signals_processed);
    }

    printf("Defensive count: %d\n", defensive_count);
    printf("All signals handled with defensive programming\n");

    return 0;
}