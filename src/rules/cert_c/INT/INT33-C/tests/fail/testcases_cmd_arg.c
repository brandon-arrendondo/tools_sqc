/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Command line argument used as divisor without validation
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <dividend> <divisor>\n", argv[0]);
        return 1;
    }

    int dividend = atoi(argv[1]);
    int divisor = atoi(argv[2]);  // No validation if user passes "0"

    int result = dividend / divisor;  // Potential divide by zero
    printf("Result: %d\n", result);
    return 0;
}