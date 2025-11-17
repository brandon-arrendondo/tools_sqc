/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t chain_depth = 0;

void handler_1(int sig);
void handler_2(int sig);
void handler_3(int sig);
void handler_4(int sig);

void handler_1(int sig) {
    chain_depth++;
    printf("Handler 1: Signal %d (depth %d)\n", sig, chain_depth);

    // VIOLATION: Chaining to next handler using signal()
    if (signal(sig, handler_2) == SIG_ERR) {
        printf("Handler 1: Failed to chain to handler 2\n");
    } else {
        printf("Handler 1: Chained to handler 2\n");
    }
}

void handler_2(int sig) {
    chain_depth++;
    printf("Handler 2: Signal %d (depth %d)\n", sig, chain_depth);

    // VIOLATION: Chaining to next handler using signal()
    if (signal(sig, handler_3) == SIG_ERR) {
        printf("Handler 2: Failed to chain to handler 3\n");
    } else {
        printf("Handler 2: Chained to handler 3\n");
    }
}

void handler_3(int sig) {
    chain_depth++;
    printf("Handler 3: Signal %d (depth %d)\n", sig, chain_depth);

    // VIOLATION: Chaining to next handler using signal()
    if (signal(sig, handler_4) == SIG_ERR) {
        printf("Handler 3: Failed to chain to handler 4\n");
    } else {
        printf("Handler 3: Chained to handler 4\n");
    }
}

void handler_4(int sig) {
    chain_depth++;
    printf("Handler 4: Signal %d (depth %d)\n", sig, chain_depth);

    // VIOLATION: Chaining back to first handler using signal()
    if (signal(sig, handler_1) == SIG_ERR) {
        printf("Handler 4: Failed to chain back to handler 1\n");
    } else {
        printf("Handler 4: Chained back to handler 1\n");
    }
}

int main() {
    printf("SIG34-C VIOLATION: Chain of handlers each calling signal()\n");
    printf("Each handler registers the next one in sequence\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, handler_1) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger handler chain\n");

    while (chain_depth < 16) {
        pause();
    }

    printf("Handler chain executed %d times\n", chain_depth);
    return 0;
}