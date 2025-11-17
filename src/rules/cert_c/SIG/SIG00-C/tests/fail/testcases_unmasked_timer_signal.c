/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t timer_count = 0;

void timer_handler(int sig) {
    timer_count++;
    printf("Timer signal %d, count = %d\n", sig, timer_count);

    // Vulnerable: long operation without masking other timer signals
    sleep(2);

    printf("Timer handler complete\n");
}

int main() {
    struct sigaction sa;
    struct itimerval timer;

    // Install handler without masking timer signals
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Not masking SIGALRM during handler execution
    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Set up repeating timer
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);

    printf("Timer started, handler can be interrupted\n");

    while (1) {
        pause();
    }

    return 0;
}