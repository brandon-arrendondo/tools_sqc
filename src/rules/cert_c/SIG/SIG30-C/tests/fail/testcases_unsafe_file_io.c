/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void file_handler(int sig) {
    // VIOLATION: File I/O functions are not async-safe
    FILE *fp = fopen("signal_log.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "Signal %d received\n", sig);
        fwrite("Additional data", 1, 15, fp);
        fflush(fp);
        fclose(fp);
    }

    // VIOLATION: fread is not async-safe
    FILE *input = fopen("/etc/passwd", "r");
    if (input != NULL) {
        char buffer[100];
        size_t bytes = fread(buffer, 1, sizeof(buffer), input);
        fclose(input);
    }
}

int main() {
    printf("Demonstrating unsafe file I/O in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, file_handler);

    printf("Send SIGUSR1 to trigger unsafe file operations\n");

    while (1) {
        pause();
    }

    return 0;
}