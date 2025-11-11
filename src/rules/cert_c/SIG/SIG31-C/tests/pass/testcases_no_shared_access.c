/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* No shared state accessed in signal handler - compliant */
void safe_handler(int sig) {
    /* Compliant: Signal handler doesn't access any shared objects */
    /* Only using local variables and function parameters */
    const char *signal_name;

    switch (sig) {
        case SIGUSR1:
            signal_name = "SIGUSR1";
            break;
        case SIGUSR2:
            signal_name = "SIGUSR2";
            break;
        case SIGTERM:
            signal_name = "SIGTERM";
            break;
        default:
            signal_name = "UNKNOWN";
            break;
    }

    /* Safe to call async-signal-safe functions */
    write(STDERR_FILENO, "Signal ", 7);
    write(STDERR_FILENO, signal_name, strlen(signal_name));
    write(STDERR_FILENO, " received\n", 10);
}

int main() {
    printf("Demonstrating signal handler with no shared object access\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, safe_handler);
    signal(SIGUSR2, safe_handler);
    signal(SIGTERM, safe_handler);

    /* Main program can safely use any variables */
    int counter = 0;
    char message[100];
    double calculation = 0.0;

    for (int i = 0; i < 30; i++) {
        counter = i * 2;
        sprintf(message, "Main program iteration %d", i);
        calculation = i * 3.14159;

        printf("Main: counter=%d, msg=%s, calc=%.2f\n",
               counter, message, calculation);

        usleep(100000);
    }

    printf("Program completed safely\n");
    return 0;
}