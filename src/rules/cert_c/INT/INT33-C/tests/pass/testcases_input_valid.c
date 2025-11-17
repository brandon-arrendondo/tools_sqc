/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: User input validation ensures divisor is not zero before division
 */

#include <stdio.h>

int main() {
    int dividend, divisor;

    printf("Enter dividend: ");
    scanf("%d", &dividend);

    do {
        printf("Enter non-zero divisor: ");
        scanf("%d", &divisor);
        if (divisor == 0) {
            printf("Error: Divisor cannot be zero. Please try again.\n");
        }
    } while (divisor == 0);

    int result = dividend / divisor;
    printf("Result: %d\n", result);
    return 0;
}