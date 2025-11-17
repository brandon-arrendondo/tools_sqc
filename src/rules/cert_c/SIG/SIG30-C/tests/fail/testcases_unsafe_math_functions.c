/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

void math_handler(int sig) {
    double x = 42.0;
    double y = 3.14;

    // VIOLATION: Mathematical functions are generally not async-safe
    double result = sin(x);
    result = cos(y);
    result = tan(x);
    result = exp(y);
    result = log(x);
    result = sqrt(x);
    result = pow(x, y);

    // VIOLATION: ceil, floor, round are not async-safe
    result = ceil(x);
    result = floor(y);
    result = round(x);

    // VIOLATION: Trigonometric functions
    result = asin(0.5);
    result = acos(0.5);
    result = atan(1.0);
    result = atan2(y, x);

    // VIOLATION: Hyperbolic functions
    result = sinh(x);
    result = cosh(x);
    result = tanh(x);

    // VIOLATION: Complex mathematical operations may use non-async-safe code
    result = fmod(x, y);
    result = ldexp(x, 2);
    result = frexp(x, NULL);
}

int main() {
    printf("Demonstrating unsafe mathematical functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, math_handler);

    printf("Send SIGUSR1 to trigger unsafe math operations\n");

    while (1) {
        pause();
    }

    return 0;
}