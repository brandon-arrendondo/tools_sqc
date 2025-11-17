/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* File-scope variables shared between handler and main */
int file_scope_counter = 0;
char file_scope_message[128] = "Initial message";
double file_scope_accumulator = 0.0;

void unsafe_handler(int sig) {
    /* Violation: Accessing file-scope variables in signal handler */
    file_scope_counter += 5;
    sprintf(file_scope_message, "Handler processed signal %d", sig);
    file_scope_accumulator += (double)sig * 0.5;

    printf("Handler: counter=%d, msg=%s, acc=%.2f\n",
           file_scope_counter, file_scope_message, file_scope_accumulator);
}

int main() {
    printf("Demonstrating unsafe file-scope variable access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 1; i <= 25; i++) {
        file_scope_counter = i;
        sprintf(file_scope_message, "Main loop iteration %d", i);
        file_scope_accumulator = i * 2.5;

        printf("Main: counter=%d, msg=%s, acc=%.2f\n",
               file_scope_counter, file_scope_message, file_scope_accumulator);

        usleep(80000);
    }

    return 0;
}