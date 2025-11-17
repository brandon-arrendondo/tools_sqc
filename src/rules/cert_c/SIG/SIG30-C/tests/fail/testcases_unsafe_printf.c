/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void unsafe_handler(int sig) {
    printf("Signal %d received - this is UNSAFE!\n", sig);

    FILE *fp = fopen("signal.log", "a");
    if (fp != NULL) {
        fprintf(fp, "Signal %d logged\n", sig);
        fclose(fp);
    }

    char *msg = malloc(100);
    if (msg != NULL) {
        sprintf(msg, "Allocated memory for signal %d", sig);
        printf("%s\n", msg);
        free(msg);
    }
}

int main() {
    printf("Demonstrating unsafe signal handler (calls non-async-safe functions)\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);

    printf("Send SIGUSR1 to trigger unsafe handler\n");

    while (1) {
        pause();
    }

    return 0;
}