/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void time_handler(int sig) {
    // VIOLATION: time() is not async-safe
    time_t current_time = time(NULL);

    // VIOLATION: ctime() is not async-safe
    char *time_str = ctime(&current_time);

    // VIOLATION: localtime() is not async-safe
    struct tm *local_tm = localtime(&current_time);

    // VIOLATION: strftime() is not async-safe
    char formatted_time[100];
    if (local_tm != NULL) {
        strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", local_tm);
    }

    // VIOLATION: gmtime() is not async-safe
    struct tm *gmt_tm = gmtime(&current_time);

    // VIOLATION: mktime() is not async-safe
    if (gmt_tm != NULL) {
        time_t reconstructed = mktime(gmt_tm);
    }
}

int main() {
    printf("Demonstrating unsafe time functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, time_handler);

    printf("Send SIGUSR1 to trigger unsafe time operations\n");

    while (1) {
        pause();
    }

    return 0;
}