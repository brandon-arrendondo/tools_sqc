/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

// Global state that demonstrates multiple violations
static FILE *global_log = NULL;
static char *global_buffer = NULL;
static size_t buffer_size = 0;

void complex_handler(int sig) {
    // VIOLATION: Multiple unsafe operations in sequence

    // 1. Dynamic memory allocation
    if (global_buffer == NULL) {
        buffer_size = 1024;
        global_buffer = malloc(buffer_size);
    } else {
        buffer_size *= 2;
        global_buffer = realloc(global_buffer, buffer_size);
    }

    if (global_buffer == NULL) {
        perror("Memory allocation failed");  // VIOLATION: perror
        exit(1);  // VIOLATION: exit instead of _exit
    }

    // 2. Complex string operations
    strcpy(global_buffer, "Signal ");
    char sig_str[20];
    sprintf(sig_str, "%d", sig);
    strcat(global_buffer, sig_str);
    strcat(global_buffer, " at ");

    // 3. Time operations
    time_t now = time(NULL);
    char *timestr = ctime(&now);
    strcat(global_buffer, timestr);

    // 4. File operations
    if (global_log == NULL) {
        global_log = fopen("/tmp/complex_signal.log", "a");
    }

    if (global_log != NULL) {
        fprintf(global_log, "%s", global_buffer);
        fflush(global_log);
    }

    // 5. Error handling
    if (ferror(global_log)) {
        char *error_msg = strerror(errno);
        printf("Log error: %s\n", error_msg);
        clearerr(global_log);
    }

    // 6. Signal manipulation
    if (sig == SIGUSR1) {
        signal(SIGUSR2, complex_handler);
    }

    // 7. Process information
    printf("PID %d handled signal %d\n", getpid(), sig);
}

int main() {
    printf("Demonstrating complex unsafe signal handler with multiple violations\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, complex_handler);
    signal(SIGUSR2, complex_handler);

    printf("Send SIGUSR1 or SIGUSR2 to trigger complex unsafe operations\n");

    while (1) {
        pause();
    }

    // Cleanup (never reached)
    if (global_log) {
        fclose(global_log);
    }
    if (global_buffer) {
        free(global_buffer);
    }

    return 0;
}