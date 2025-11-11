/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

void logging_handler(int sig) {
    // VIOLATION: syslog() and related functions are not async-safe
    openlog("signal_test", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Signal %d received", sig);
    syslog(LOG_WARNING, "This is unsafe logging");
    closelog();

    // VIOLATION: Custom logging using file operations
    FILE *logfile = fopen("/tmp/signal.log", "a");
    if (logfile != NULL) {
        fprintf(logfile, "Signal handler called with %d\n", sig);
        fflush(logfile);
        fclose(logfile);
    }

    // VIOLATION: Using buffered I/O for logging
    char log_msg[256];
    snprintf(log_msg, sizeof(log_msg), "Signal %d at process %d\n", sig, getpid());

    // Writing to stderr with buffered functions
    fputs(log_msg, stderr);
    fputc('\n', stderr);

    // VIOLATION: Complex logging with timestamps
    time_t now = time(NULL);
    char *timestr = ctime(&now);
    printf("LOG [%s]: Signal %d\n", timestr, sig);
}

int main() {
    printf("Demonstrating unsafe logging functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, logging_handler);

    printf("Send SIGUSR1 to trigger unsafe logging operations\n");

    while (1) {
        pause();
    }

    return 0;
}