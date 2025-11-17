/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Compliant: Using proper signal masking to protect shared access */
volatile sig_atomic_t signal_pending = 0;
volatile sig_atomic_t signal_value = 0;

/* Shared data protected by signal masking */
static int protected_counter = 0;
static char protected_message[256];
static double protected_calculation = 0.0;

void safe_signal_handler(int sig) {
    /* Compliant: Only setting atomic flags */
    signal_pending = 1;
    signal_value = sig;
}

void protected_update_data(int new_value, const char *message, double calc) {
    /* Properly mask signals before accessing shared data */
    sigset_t mask, old_mask;

    /* Block all signals while updating shared data */
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);

    /* Critical section - safe to modify shared data */
    protected_counter = new_value;
    snprintf(protected_message, sizeof(protected_message), "%s", message);
    protected_calculation = calc;

    /* Restore original signal mask */
    sigprocmask(SIG_SETMASK, &old_mask, NULL);
}

void protected_read_data(int *counter, char *message, double *calc) {
    /* Properly mask signals before reading shared data */
    sigset_t mask, old_mask;

    /* Block all signals while reading shared data */
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);

    /* Critical section - safe to read shared data */
    *counter = protected_counter;
    snprintf(message, 256, "%s", protected_message);
    *calc = protected_calculation;

    /* Restore original signal mask */
    sigprocmask(SIG_SETMASK, &old_mask, NULL);
}

int main() {
    printf("Demonstrating safe signal handling with proper signal masking\n");
    printf("PID: %d\n", getpid());

    /* Install signal handler */
    signal(SIGUSR1, safe_signal_handler);
    signal(SIGUSR2, safe_signal_handler);

    /* Initialize protected data safely */
    protected_update_data(0, "Initial state", 0.0);

    printf("Signal handler installed with proper masking protection\n");
    printf("Send SIGUSR1 or SIGUSR2 to test signal handling\n");

    for (int i = 0; i < 30; i++) {
        /* Check for pending signals */
        if (signal_pending) {
            signal_pending = 0;  /* Reset flag */
            int received_signal = signal_value;

            printf("Signal %d received and processed safely\n", received_signal);

            /* Process the signal by updating protected data */
            char signal_message[256];
            snprintf(signal_message, sizeof(signal_message),
                     "Updated by signal %d at iteration %d", received_signal, i);

            protected_update_data(i + 1000, signal_message, received_signal * 3.14);
        }

        /* Normal processing with protected data access */
        char temp_message[256];
        int temp_counter;
        double temp_calc;

        /* Safe read of protected data */
        protected_read_data(&temp_counter, temp_message, &temp_calc);

        printf("Main[%d]: counter=%d, calc=%.2f, msg=%s\n",
               i, temp_counter, temp_calc, temp_message);

        /* Update protected data safely */
        char main_message[256];
        snprintf(main_message, sizeof(main_message),
                 "Main program iteration %d", i);

        protected_update_data(i, main_message, i * 2.718);

        /* Simulate some work */
        usleep(150000);

        /* Demonstrate signal masking during critical operations */
        if (i % 10 == 9) {
            printf("Performing critical operation with signals blocked...\n");

            sigset_t mask, old_mask;
            sigfillset(&mask);
            sigprocmask(SIG_BLOCK, &mask, &old_mask);

            /* Critical operation - signals are blocked */
            for (int j = 0; j < 5; j++) {
                protected_counter += j;
                protected_calculation += j * 0.1;
                usleep(50000);  /* Simulate work */
            }

            printf("Critical operation completed, restoring signal mask\n");
            sigprocmask(SIG_SETMASK, &old_mask, NULL);
        }
    }

    /* Final read of protected data */
    int final_counter;
    char final_message[256];
    double final_calc;

    protected_read_data(&final_counter, final_message, &final_calc);

    printf("Program completed safely with signal masking protection\n");
    printf("Final state: counter=%d, calc=%.2f, msg=%s\n",
           final_counter, final_calc, final_message);

    return 0;
}