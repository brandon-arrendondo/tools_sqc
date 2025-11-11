/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char global_log_buffer[1024];
char global_error_msg[256];
char *global_dynamic_string = NULL;

void unsafe_handler(int sig) {
    /* Violation: Accessing string buffers in signal handler */
    strcat(global_log_buffer, "Signal occurred! ");

    sprintf(global_error_msg, "Error: Signal %d interrupted operation", sig);

    if (global_dynamic_string) {
        strcat(global_dynamic_string, "[SIGNAL]");
    }

    printf("Handler: log_len=%zu, error=%s\n",
           strlen(global_log_buffer), global_error_msg);
}

int main() {
    printf("Demonstrating unsafe string buffer access in signal handler\n");
    printf("PID: %d\n", getpid());

    strcpy(global_log_buffer, "Started: ");
    strcpy(global_error_msg, "No errors");

    global_dynamic_string = malloc(512);
    if (global_dynamic_string) {
        strcpy(global_dynamic_string, "Dynamic content ");
    }

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 15; i++) {
        char temp[64];
        sprintf(temp, "Step %d ", i);
        strcat(global_log_buffer, temp);

        sprintf(global_error_msg, "Processing step %d", i);

        if (global_dynamic_string) {
            sprintf(temp, "[%d]", i);
            strcat(global_dynamic_string, temp);
        }

        printf("Main: log_len=%zu, error=%s\n",
               strlen(global_log_buffer), global_error_msg);

        usleep(200000);
    }

    free(global_dynamic_string);
    return 0;
}