/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Time calculation with zero duration causing divide-by-zero error
 */

#include <stdio.h>

int main() {
    int distance = 100;
    int time = 0;  // Zero time duration

    double speed = (double)distance / time;  // Divide by zero
    printf("Speed: %.2f units per time\n", speed);
    return 0;
}