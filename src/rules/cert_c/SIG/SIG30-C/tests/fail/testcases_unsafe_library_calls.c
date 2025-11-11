/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void dangerous_handler(int sig) {
    time_t current_time = time(NULL);
    char *time_str = ctime(&current_time);

    printf("Signal received at: %s", time_str);

    char buffer[256];
    strcpy(buffer, "Signal handler executed");
    strcat(buffer, " at time: ");
    strcat(buffer, time_str);

    FILE *log = fopen("/tmp/signal.log", "w");
    if (log) {
        fprintf(log, "%s", buffer);
        fclose(log);
    }

    exit(EXIT_SUCCESS);
}

int main() {
    printf("Demonstrating multiple unsafe functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGTERM, dangerous_handler);
    signal(SIGINT, dangerous_handler);

    printf("Press Ctrl+C or send SIGTERM to trigger dangerous handler\n");

    while (1) {
        pause();
    }

    return 0;
}