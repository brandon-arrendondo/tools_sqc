/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Compliant: Signal handlers that only set flags using volatile sig_atomic_t */
volatile sig_atomic_t debug_flag = 0;
volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t config_reload_flag = 0;
volatile sig_atomic_t status_request_flag = 0;

void debug_signal_handler(int sig) {
    /* Compliant: Only setting a volatile sig_atomic_t flag */
    debug_flag = 1;
}

void shutdown_signal_handler(int sig) {
    /* Compliant: Only setting a volatile sig_atomic_t flag */
    shutdown_flag = 1;
}

void config_signal_handler(int sig) {
    /* Compliant: Only setting a volatile sig_atomic_t flag */
    config_reload_flag = 1;
}

void status_signal_handler(int sig) {
    /* Compliant: Only setting a volatile sig_atomic_t flag */
    status_request_flag = 1;
}

int main() {
    printf("Demonstrating safe signal handlers that only set flags\n");
    printf("PID: %d\n", getpid());

    /* Install signal handlers */
    signal(SIGUSR1, debug_signal_handler);
    signal(SIGUSR2, status_signal_handler);
    signal(SIGTERM, shutdown_signal_handler);
    signal(SIGHUP, config_signal_handler);

    /* Main program state (not accessed by signal handlers) */
    int debug_mode = 0;
    int iteration_count = 0;
    char config_file[256] = "default.conf";
    char status_info[512];

    printf("Handlers installed. Send signals:\n");
    printf("  SIGUSR1 - Toggle debug mode\n");
    printf("  SIGUSR2 - Show status\n");
    printf("  SIGHUP  - Reload config\n");
    printf("  SIGTERM - Shutdown\n");

    while (!shutdown_flag) {
        iteration_count++;

        /* Check debug flag */
        if (debug_flag) {
            debug_flag = 0;  /* Reset flag */
            debug_mode = !debug_mode;
            printf("Debug mode %s\n", debug_mode ? "ENABLED" : "DISABLED");
        }

        /* Check status request flag */
        if (status_request_flag) {
            status_request_flag = 0;  /* Reset flag */
            sprintf(status_info,
                    "Status: iteration=%d, debug=%s, config=%s",
                    iteration_count,
                    debug_mode ? "ON" : "OFF",
                    config_file);
            printf("STATUS: %s\n", status_info);
        }

        /* Check config reload flag */
        if (config_reload_flag) {
            config_reload_flag = 0;  /* Reset flag */
            sprintf(config_file, "reloaded_%d.conf", iteration_count);
            printf("Config reloaded: %s\n", config_file);
        }

        /* Normal program work */
        if (debug_mode) {
            printf("DEBUG: Main loop iteration %d\n", iteration_count);
        } else if (iteration_count % 10 == 0) {
            printf("Main: iteration %d\n", iteration_count);
        }

        usleep(200000);  /* 200ms */
    }

    printf("Shutdown flag detected, exiting gracefully\n");
    return 0;
}