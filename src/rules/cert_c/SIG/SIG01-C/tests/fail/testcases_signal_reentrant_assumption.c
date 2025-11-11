/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t reentrant_count = 0;

void reentrant_handler(int sig) {
    reentrant_count++;

    /* Non-reentrant function usage - VIOLATION */
    char buffer[100];
    sprintf(buffer, "Reentrant handler call %d\n", reentrant_count);
    printf("%s", buffer);  /* printf is not async-signal-safe */

    /* Assumes malloc is safe in signal handler */
    char* dynamic_msg = malloc(50);
    if (dynamic_msg) {
        strcpy(dynamic_msg, "Dynamic message");  /* strcpy not safe */
        printf("%s\n", dynamic_msg);
        free(dynamic_msg);  /* free not safe in signal handler */
    }
}

int main() {
    printf("FAIL: Signal reentrancy and safety assumptions\n");

    signal(SIGUSR2, reentrant_handler);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR2 to test reentrant signal handling\n");

    /* Send multiple signals */
    int i;
    for (i = 0; i < 3; i++) {
        raise(SIGUSR2);
        usleep(100000);
    }

    sleep(1);

    printf("Reentrant calls: %d\n", reentrant_count);
    printf("Code uses non-async-signal-safe functions in handler\n");

    return 0;
}