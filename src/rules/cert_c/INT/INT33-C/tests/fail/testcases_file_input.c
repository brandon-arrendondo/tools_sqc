/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Reading divisor from file without validation for zero
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("data.txt", "w");
    if (file) {
        fprintf(file, "10 0");  // Write data including zero
        fclose(file);
    }

    file = fopen("data.txt", "r");
    if (file) {
        int dividend, divisor;
        fscanf(file, "%d %d", &dividend, &divisor);
        fclose(file);

        int result = dividend / divisor;  // No validation of file data
        printf("Result: %d\n", result);
    }
    return 0;
}