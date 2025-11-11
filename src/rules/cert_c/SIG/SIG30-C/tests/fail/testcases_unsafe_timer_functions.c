/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

void timer_handler(int sig) {
    // VIOLATION: gettimeofday() is not guaranteed async-safe
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // VIOLATION: settimeofday() is not async-safe
    // settimeofday(&tv, NULL);  // Commented out to avoid system time changes

    // VIOLATION: clock_gettime() may not be async-safe on all systems
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // VIOLATION: clock_settime() is not async-safe
    // clock_settime(CLOCK_REALTIME, &ts);  // Commented out

    // VIOLATION: nanosleep() is not async-safe
    struct timespec sleep_time = {0, 100000000};  // 100ms
    nanosleep(&sleep_time, NULL);

    // VIOLATION: Timer creation and manipulation
    timer_t timerid;
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR2;

    // VIOLATION: timer_create() is not async-safe
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    // VIOLATION: timer_settime() is not async-safe
    struct itimerspec its;
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    timer_settime(timerid, 0, &its, NULL);

    // VIOLATION: timer_gettime() is not async-safe
    timer_gettime(timerid, &its);

    // VIOLATION: timer_delete() is not async-safe
    timer_delete(timerid);
}

int main() {
    printf("Demonstrating unsafe timer functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, timer_handler);

    printf("Send SIGUSR1 to trigger unsafe timer operations\n");

    while (1) {
        pause();
    }

    return 0;
}