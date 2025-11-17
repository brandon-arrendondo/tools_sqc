/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Compliant: Signal handlers using only atomic operations with sig_atomic_t */
volatile sig_atomic_t signal_counter = 0;
volatile sig_atomic_t last_signal_type = 0;
volatile sig_atomic_t error_occurred = 0;
volatile sig_atomic_t processing_active = 0;

void increment_signal_handler(int sig) {
    /* Compliant: Only atomic operations on volatile sig_atomic_t */
    signal_counter++;
    last_signal_type = sig;
}

void error_signal_handler(int sig) {
    /* Compliant: Only atomic operations on volatile sig_atomic_t */
    error_occurred = 1;
    last_signal_type = sig;
    signal_counter++;
}

void toggle_signal_handler(int sig) {
    /* Compliant: Only atomic operations on volatile sig_atomic_t */
    processing_active = (processing_active == 0) ? 1 : 0;
    last_signal_type = sig;
    signal_counter++;
}

int main() {
    printf("Demonstrating safe signal handlers with atomic operations only\n");
    printf("PID: %d\n", getpid());

    /* Install signal handlers */
    signal(SIGUSR1, increment_signal_handler);
    signal(SIGUSR2, error_signal_handler);
    signal(SIGTERM, toggle_signal_handler);

    /* Main program variables (not accessed by signal handlers) */
    int main_counter = 0;
    int total_errors_handled = 0;
    int processing_cycles = 0;
    char operation_log[512];

    printf("Signal handlers installed:\n");
    printf("  SIGUSR1 - Increment counter\n");
    printf("  SIGUSR2 - Set error flag\n");
    printf("  SIGTERM - Toggle processing\n");

    /* Store previous values to detect changes */
    sig_atomic_t prev_signal_counter = 0;
    sig_atomic_t prev_error_occurred = 0;
    sig_atomic_t prev_processing_active = 0;

    for (int i = 0; i < 50; i++) {
        main_counter = i;

        /* Atomically read current signal state */
        sig_atomic_t current_signals = signal_counter;
        sig_atomic_t current_error = error_occurred;
        sig_atomic_t current_processing = processing_active;
        sig_atomic_t current_last_signal = last_signal_type;

        /* Detect and handle signal counter changes */
        if (current_signals != prev_signal_counter) {
            printf("Signal received: type=%d, total_count=%d\n",
                   (int)current_last_signal, (int)current_signals);
            prev_signal_counter = current_signals;
        }

        /* Detect and handle error flag changes */
        if (current_error != prev_error_occurred) {
            if (current_error) {
                total_errors_handled++;
                printf("Error condition detected by signal, total errors: %d\n",
                       total_errors_handled);
                /* Reset error flag atomically */
                error_occurred = 0;
            }
            prev_error_occurred = current_error;
        }

        /* Detect and handle processing state changes */
        if (current_processing != prev_processing_active) {
            printf("Processing state changed to: %s\n",
                   current_processing ? "ACTIVE" : "INACTIVE");
            prev_processing_active = current_processing;
        }

        /* Main program work based on atomic flags */
        if (current_processing) {
            processing_cycles++;
            sprintf(operation_log, "Active processing cycle %d", processing_cycles);
        } else {
            sprintf(operation_log, "Inactive - waiting for activation");
        }

        if (i % 5 == 0) {
            printf("Main: iter=%d, signals=%d, errors_handled=%d, state=%s\n",
                   main_counter, (int)current_signals, total_errors_handled,
                   current_processing ? "ACTIVE" : "INACTIVE");
        }

        usleep(150000);
    }

    printf("Final state: signals=%d, processing=%s, errors_handled=%d\n",
           (int)signal_counter,
           processing_active ? "ACTIVE" : "INACTIVE",
           total_errors_handled);

    return 0;
}