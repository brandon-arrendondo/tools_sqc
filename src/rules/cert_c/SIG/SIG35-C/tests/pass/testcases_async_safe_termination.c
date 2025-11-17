/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_received = 0;

void async_safe_handler(int sig) {
    /* Only use async-signal-safe functions */
    signal_received = sig;

    /* Use only async-signal-safe write() instead of printf() */
    const char msg1[] = "COMPUTATIONAL EXCEPTION: Signal ";
    const char msg2[] = " received\n";
    const char msg3[] = "Using only async-signal-safe operations\n";
    const char msg4[] = "Terminating safely with abort()\n";

    write(STDERR_FILENO, msg1, sizeof(msg1) - 1);

    /* Convert signal number to character (simple case) */
    if (sig >= 0 && sig <= 9) {
        char sig_char = '0' + sig;
        write(STDERR_FILENO, &sig_char, 1);
    } else {
        write(STDERR_FILENO, "X", 1);
    }

    write(STDERR_FILENO, msg2, sizeof(msg2) - 1);
    write(STDERR_FILENO, msg3, sizeof(msg3) - 1);
    write(STDERR_FILENO, msg4, sizeof(msg4) - 1);

    abort(); /* COMPLIANT: async-signal-safe termination, never returns */
}

int main() {
    printf("Demonstrating async-signal-safe termination only\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, async_safe_handler);
    signal(SIGSEGV, async_safe_handler);

    printf("Signal received: %d\n", signal_received);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This line should never be reached\n");
    return 0;
}