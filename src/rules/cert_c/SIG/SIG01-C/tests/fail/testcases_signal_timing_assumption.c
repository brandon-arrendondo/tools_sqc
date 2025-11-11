/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t timing_count = 0;
time_t start_time;

void timing_handler(int sig) {
    timing_count++;
    time_t current = time(NULL);
    printf("Signal %d at time %ld (elapsed: %ld)\n",
           timing_count, current, current - start_time);
}

int main() {
    printf("FAIL: Signal timing and delivery assumptions\n");

    signal(SIGALRM, timing_handler);

    printf("PID: %d\n", getpid());
    start_time = time(NULL);

    /* Assumes precise timing of signal delivery */
    printf("Setting alarms with precise timing expectations\n");

    alarm(1);
    sleep(1);

    alarm(1);
    sleep(1);

    /* Reset handler and set another alarm */
    signal(SIGALRM, timing_handler);  /* Assumes this maintains timing */
    alarm(1);
    sleep(1);

    printf("Timing signals received: %d\n", timing_count);
    printf("Assumes predictable signal timing across handler resets\n");

    return 0;
}