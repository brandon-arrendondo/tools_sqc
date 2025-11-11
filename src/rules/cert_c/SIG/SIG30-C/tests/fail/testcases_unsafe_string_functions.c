/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char global_buffer[256];

void string_handler(int sig) {
    char temp[100];

    // VIOLATION: strcpy, strcat, strlen are not async-safe
    strcpy(temp, "Signal ");
    char sig_str[20];
    sprintf(sig_str, "%d", sig);  // sprintf also unsafe
    strcat(temp, sig_str);
    strcat(temp, " received");

    size_t len = strlen(temp);

    // VIOLATION: memcpy, memmove are not guaranteed async-safe
    memcpy(global_buffer, temp, len + 1);

    // VIOLATION: strcmp, strncmp are not async-safe
    if (strcmp(temp, "Signal 10 received") == 0) {
        strcpy(global_buffer, "SIGUSR1 detected");
    }
}

int main() {
    printf("Demonstrating unsafe string operations in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, string_handler);

    printf("Send SIGUSR1 to trigger unsafe string operations\n");

    while (1) {
        pause();
    }

    return 0;
}