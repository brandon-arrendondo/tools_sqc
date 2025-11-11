/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t state_count = 0;
volatile sig_atomic_t handler_active = 1;

void state_handler(int sig) {
    state_count++;
    printf("State handler called: %d\n", state_count);

    if (state_count == 2) {
        handler_active = 0;
        printf("Handler deactivating itself\n");
    }
}

int main() {
    printf("FAIL: Signal handler state management assumption\n");

    signal(SIGUSR1, state_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 multiple times\n");

    /* Send first signal */
    raise(SIGUSR1);
    sleep(1);

    /* Assumes handler is still active despite internal state changes */
    printf("Handler active flag: %d\n", handler_active);
    printf("Sending second signal, assuming handler persists\n");

    raise(SIGUSR1);
    sleep(1);

    /* Assumes handler state is preserved */
    printf("Sending third signal\n");
    raise(SIGUSR1);
    sleep(1);

    printf("Final state count: %d\n", state_count);
    printf("Code assumes handler state management works consistently\n");

    return 0;
}