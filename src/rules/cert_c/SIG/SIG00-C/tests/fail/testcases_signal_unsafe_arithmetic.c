/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    double real;
    double imaginary;
} complex_t;

volatile complex_t shared_complex = {0.0, 0.0};
volatile sig_atomic_t operation_count = 0;

void math_handler(int sig) {
    operation_count++;

    printf("Handler: Signal %d, performing complex arithmetic\n", sig);

    // Violation: Complex arithmetic operations without masking
    // can be interrupted, leading to inconsistent state
    shared_complex.real += 1.0;

    // Simulate floating-point computation
    for (int i = 0; i < 1000; i++) {
        shared_complex.real = shared_complex.real * 1.001;

        // Create interruption opportunity
        if (i % 100 == 0) {
            usleep(1000);
        }
    }

    shared_complex.imaginary += 0.5;

    // More complex operations
    double magnitude = shared_complex.real * shared_complex.real +
                      shared_complex.imaginary * shared_complex.imaginary;

    printf("Handler: Complex = (%f, %f), magnitude^2 = %f\n",
           shared_complex.real, shared_complex.imaginary, magnitude);

    // Normalize (vulnerable operation)
    if (magnitude > 0) {
        shared_complex.real /= magnitude;
        usleep(5000); // Vulnerability window
        shared_complex.imaginary /= magnitude;
    }

    printf("Handler: Operation %d complete\n", operation_count);
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = math_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Floating-point operations vulnerable to interruption
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to interrupt floating-point operations\n");

    while (1) {
        printf("Main: Complex number = (%f, %f), ops = %d\n",
               shared_complex.real, shared_complex.imaginary, operation_count);

        // Check for obvious corruption
        if (shared_complex.real != shared_complex.real ||  // NaN check
            shared_complex.imaginary != shared_complex.imaginary) {
            printf("Main: ERROR - NaN detected in complex number!\n");
        }

        sleep(2);
    }

    return 0;
}