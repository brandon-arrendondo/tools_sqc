/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t custom_count = 0;

void custom_handler(int sig) {
    custom_count++;
    printf("Custom handler: %d\n", custom_count);
}

int main() {
    printf("FAIL: Signal default behavior assumption\n");

    /* Set custom handler */
    signal(SIGTERM, custom_handler);

    printf("PID: %d\n", getpid());
    printf("Custom handler set for SIGTERM\n");

    /* Test custom handler */
    raise(SIGTERM);
    sleep(1);

    /* Reset to default, but code assumes custom handler persists */
    signal(SIGTERM, SIG_DFL);
    printf("Reset SIGTERM to default\n");

    /* Code incorrectly assumes custom handler is still active */
    printf("Code assumes custom handler still active (incorrect)\n");
    printf("Sending SIGTERM again - will use default behavior\n");

    /* This would terminate the program with default handler */
    printf("If SIGTERM sent now, program would terminate\n");
    printf("Custom count: %d (handler no longer active)\n", custom_count);

    return 0;
}