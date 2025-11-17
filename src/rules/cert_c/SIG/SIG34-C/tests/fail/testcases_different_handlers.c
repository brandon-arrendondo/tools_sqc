/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t handler_switch_count = 0;

void handler_a(int sig);
void handler_b(int sig);
void handler_c(int sig);

void handler_a(int sig) {
    printf("Handler A processing signal %d\n", sig);
    handler_switch_count++;

    // VIOLATION: Switching to different handler function using signal()
    if (signal(sig, handler_b) == SIG_ERR) {
        printf("Failed to switch to handler B\n");
    } else {
        printf("Switched to handler B\n");
    }
}

void handler_b(int sig) {
    printf("Handler B processing signal %d\n", sig);
    handler_switch_count++;

    // VIOLATION: Switching to different handler function using signal()
    if (signal(sig, handler_c) == SIG_ERR) {
        printf("Failed to switch to handler C\n");
    } else {
        printf("Switched to handler C\n");
    }
}

void handler_c(int sig) {
    printf("Handler C processing signal %d\n", sig);
    handler_switch_count++;

    // VIOLATION: Switching back to handler A using signal()
    if (signal(sig, handler_a) == SIG_ERR) {
        printf("Failed to switch to handler A\n");
    } else {
        printf("Switched back to handler A\n");
    }
}

int main() {
    printf("SIG34-C VIOLATION: Handler switching between different functions\n");
    printf("Each handler uses signal() to register a different handler\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGALRM, handler_a) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Triggering alarm signals to see handler rotation\n");

    for (int i = 0; i < 8; i++) {
        alarm(1);
        sleep(2);
    }

    printf("Handler switches completed: %d\n", handler_switch_count);
    return 0;
}