/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t loop_counter = 0;
volatile sig_atomic_t exit_flag = 0;

void loop_handler(int sig) {
    loop_counter++;
    printf("Loop iteration %d\n", loop_counter);

    if (loop_counter >= 10) {
        exit_flag = 1;
        printf("Setting exit flag\n");
    }
}

int main() {
    printf("FAIL: Loop relying on signal handler persistence\n");

    signal(SIGALRM, loop_handler);

    printf("PID: %d\n", getpid());
    printf("Starting signal-driven loop\n");

    /* Set repeating alarm */
    alarm(1);

    /* Loop assumes handler will keep firing alarms */
    while (!exit_flag) {
        pause();
        alarm(1);  /* Re-arm alarm, assuming handler is still active */
    }

    printf("Loop completed with counter: %d\n", loop_counter);
    printf("May fail if handler resets and stops re-arming alarm\n");

    return 0;
}