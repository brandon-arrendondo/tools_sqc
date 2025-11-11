/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

FILE *error_log = NULL;

void logging_exception_handler(int sig) {
    time_t now = time(NULL);
    char *timestamp = ctime(&now);

    printf("Exception handler: Logging error and returning\n");

    /* Attempt to log the error */
    if (error_log != NULL) {
        fprintf(error_log, "[%s] Signal %d received\n", timestamp, sig);
        fflush(error_log);
    }

    printf("Error logged, attempting to continue (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing exception handler that logs errors and returns\n");
    printf("PID: %d\n", getpid());

    error_log = fopen("/tmp/claude/error.log", "w");

    signal(SIGFPE, logging_exception_handler);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This represents undefined behavior\n");

    if (error_log) fclose(error_log);
    return 0;
}