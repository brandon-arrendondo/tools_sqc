/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Compliant: Signal handlers with completely isolated state */
volatile sig_atomic_t handler1_calls = 0;
volatile sig_atomic_t handler2_calls = 0;
volatile sig_atomic_t handler3_calls = 0;

void isolated_handler1(int sig) {
    /* Compliant: Only accesses its own isolated sig_atomic_t variable */
    handler1_calls++;

    /* All other operations use only local variables or async-signal-safe functions */
    const char msg[] = "Handler1 called\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

void isolated_handler2(int sig) {
    /* Compliant: Only accesses its own isolated sig_atomic_t variable */
    handler2_calls++;

    /* Local computation using only function parameters and local variables */
    int local_calc = sig * 2;
    char buffer[32];
    int len = 0;

    /* Simple number-to-string conversion (async-signal-safe) */
    if (local_calc == 0) {
        buffer[0] = '0';
        len = 1;
    } else {
        int temp = local_calc;
        while (temp > 0) {
            buffer[len++] = '0' + (temp % 10);
            temp /= 10;
        }
        /* Reverse the string */
        for (int i = 0; i < len / 2; i++) {
            char c = buffer[i];
            buffer[i] = buffer[len - 1 - i];
            buffer[len - 1 - i] = c;
        }
    }

    write(STDERR_FILENO, "Handler2: ", 10);
    write(STDERR_FILENO, buffer, len);
    write(STDERR_FILENO, "\n", 1);
}

void isolated_handler3(int sig) {
    /* Compliant: Only accesses its own isolated sig_atomic_t variable */
    handler3_calls++;

    /* Uses only async-signal-safe operations */
    if (sig == SIGTERM) {
        const char msg[] = "Handler3: Termination signal\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
    } else {
        const char msg[] = "Handler3: Other signal\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
    }
}

int main() {
    printf("Demonstrating signal handlers with completely isolated state\n");
    printf("PID: %d\n", getpid());

    /* Install signal handlers - each has isolated state */
    signal(SIGUSR1, isolated_handler1);
    signal(SIGUSR2, isolated_handler2);
    signal(SIGTERM, isolated_handler3);

    /* Main program state - completely separate from signal handlers */
    int main_counter = 0;
    int main_calculations[100];
    char main_status[256];
    double main_average = 0.0;

    printf("Signal handlers with isolated state installed:\n");
    printf("  SIGUSR1 - Isolated handler 1\n");
    printf("  SIGUSR2 - Isolated handler 2\n");
    printf("  SIGTERM - Isolated handler 3\n");

    for (int i = 0; i < 40; i++) {
        main_counter = i;
        main_calculations[i % 100] = i * i;

        /* Calculate running average */
        int sum = 0;
        int count = (i < 100) ? i + 1 : 100;
        for (int j = 0; j < count; j++) {
            sum += main_calculations[j];
        }
        main_average = (double)sum / count;

        sprintf(main_status, "Main iteration %d: avg=%.2f", i, main_average);

        /* Safely read handler call counts (atomic reads) */
        sig_atomic_t h1_calls = handler1_calls;
        sig_atomic_t h2_calls = handler2_calls;
        sig_atomic_t h3_calls = handler3_calls;

        printf("Main: %s | Handler calls: H1=%d, H2=%d, H3=%d\n",
               main_status, (int)h1_calls, (int)h2_calls, (int)h3_calls);

        /* Main program can safely modify its own state */
        if (i % 10 == 9) {
            printf("Main: Checkpoint reached at iteration %d\n", i);
            /* Reset some local state */
            for (int j = 0; j < 10; j++) {
                main_calculations[j] = 0;
            }
        }

        usleep(150000);
    }

    /* Final status - safe to read atomic variables */
    sig_atomic_t final_h1 = handler1_calls;
    sig_atomic_t final_h2 = handler2_calls;
    sig_atomic_t final_h3 = handler3_calls;

    printf("Program completed with isolated signal handler state\n");
    printf("Final handler call counts: H1=%d, H2=%d, H3=%d\n",
           (int)final_h1, (int)final_h2, (int)final_h3);
    printf("Main program final average: %.2f\n", main_average);

    return 0;
}