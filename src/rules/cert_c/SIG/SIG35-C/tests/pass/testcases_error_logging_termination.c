/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

void safe_logging_handler(int sig) {
    /* Use only async-signal-safe functions for logging */
    const char log_prefix[] = "FATAL ERROR [";
    const char log_suffix[] = "]: Computational exception signal ";
    const char log_end[] = " - Terminating process\n";

    /* Write timestamp and error information using async-safe functions */
    write(STDERR_FILENO, log_prefix, sizeof(log_prefix) - 1);

    /* Get PID for logging (async-safe) */
    pid_t pid = getpid();
    char pid_str[16];
    int pid_len = 0;
    if (pid > 0) {
        /* Simple integer to string conversion */
        int temp_pid = pid;
        char temp_str[16];
        int temp_len = 0;

        do {
            temp_str[temp_len++] = '0' + (temp_pid % 10);
            temp_pid /= 10;
        } while (temp_pid > 0 && temp_len < 15);

        /* Reverse the string */
        for (int i = 0; i < temp_len; i++) {
            pid_str[pid_len++] = temp_str[temp_len - 1 - i];
        }
    }

    write(STDERR_FILENO, pid_str, pid_len);
    write(STDERR_FILENO, log_suffix, sizeof(log_suffix) - 1);

    /* Write signal number */
    char sig_str[4];
    int sig_len = 0;
    int temp_sig = sig;

    do {
        sig_str[sig_len++] = '0' + (temp_sig % 10);
        temp_sig /= 10;
    } while (temp_sig > 0 && sig_len < 3);

    /* Reverse signal string */
    for (int i = 0; i < sig_len / 2; i++) {
        char temp = sig_str[i];
        sig_str[i] = sig_str[sig_len - 1 - i];
        sig_str[sig_len - 1 - i] = temp;
    }

    write(STDERR_FILENO, sig_str, sig_len);
    write(STDERR_FILENO, log_end, sizeof(log_end) - 1);

    abort(); /* COMPLIANT: Terminate after safe logging, never returns */
}

int main() {
    printf("Demonstrating proper error logging before termination\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, safe_logging_handler);
    signal(SIGSEGV, safe_logging_handler);

    printf("Triggering segmentation fault...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This line should never be reached\n");
    return 0;
}