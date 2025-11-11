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
#include <errno.h>

typedef struct {
    int error_code;
    char error_message[256];
    char error_context[128];
    int error_count;
    char last_function[64];
    int severity_level;
} error_state_t;

typedef struct {
    char diagnostic_buffer[1024];
    int debug_level;
    char log_entries[10][128];
    int log_index;
    double performance_metrics[5];
} diagnostic_info_t;

error_state_t global_error_state = {0};
diagnostic_info_t global_diagnostics = {0};

void unsafe_handler(int sig) {
    /* Violation: Accessing global error state and diagnostic information in signal handler */

    global_error_state.error_code = sig + 1000;
    sprintf(global_error_state.error_message, "Signal %d caused system error", sig);
    strcpy(global_error_state.error_context, "signal_handler");
    global_error_state.error_count++;
    strcpy(global_error_state.last_function, "unsafe_handler");
    global_error_state.severity_level = 3;  /* High severity */

    /* Update diagnostics */
    sprintf(global_diagnostics.diagnostic_buffer, "ERROR: Signal %d at handler, count=%d",
            sig, global_error_state.error_count);
    global_diagnostics.debug_level = 4;  /* Emergency debug */

    /* Add log entry */
    sprintf(global_diagnostics.log_entries[global_diagnostics.log_index],
            "Handler error: sig=%d, code=%d", sig, global_error_state.error_code);
    global_diagnostics.log_index = (global_diagnostics.log_index + 1) % 10;

    /* Update performance metrics */
    for (int i = 0; i < 5; i++) {
        global_diagnostics.performance_metrics[i] += 0.1;
    }

    printf("Handler: error_code=%d, count=%d, severity=%d, msg=%s\n",
           global_error_state.error_code, global_error_state.error_count,
           global_error_state.severity_level, global_error_state.error_message);
}

int main() {
    printf("Demonstrating unsafe error state access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 20; i++) {
        /* Simulate various error conditions */
        global_error_state.error_code = i * 10;
        sprintf(global_error_state.error_message, "Main loop error %d", i);
        strcpy(global_error_state.error_context, "main_function");
        global_error_state.error_count = i;
        strcpy(global_error_state.last_function, "main");
        global_error_state.severity_level = (i % 3) + 1;

        /* Update diagnostics */
        sprintf(global_diagnostics.diagnostic_buffer, "Main processing iteration %d", i);
        global_diagnostics.debug_level = i % 4;

        /* Add log entry */
        sprintf(global_diagnostics.log_entries[global_diagnostics.log_index],
                "Main iteration %d, error_code=%d", i, global_error_state.error_code);
        global_diagnostics.log_index = (global_diagnostics.log_index + 1) % 10;

        /* Update performance metrics */
        for (int j = 0; j < 5; j++) {
            global_diagnostics.performance_metrics[j] = i * 0.5 + j;
        }

        printf("Main: error_code=%d, count=%d, severity=%d, msg=%s\n",
               global_error_state.error_code, global_error_state.error_count,
               global_error_state.severity_level, global_error_state.error_message);

        usleep(150000);
    }

    return 0;
}