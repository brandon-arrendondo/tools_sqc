/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Safe division macro with built-in zero checking functionality
 */

#include <stdio.h>

#define SAFE_DIVIDE(a, b) \
    ((b) != 0 ? (a) / (b) : \
     (printf("Error: Division by zero in macro\n"), 0))

#define SAFE_MODULO(a, b) \
    ((b) != 0 ? (a) % (b) : \
     (printf("Error: Modulo by zero in macro\n"), 0))

int main() {
    int x = 20, y = 4, z = 0;

    printf("20 / 4 = %d\n", SAFE_DIVIDE(x, y));
    printf("20 %% 4 = %d\n", SAFE_MODULO(x, y));

    printf("Attempting division by zero:\n");
    printf("20 / 0 = %d\n", SAFE_DIVIDE(x, z));
    printf("20 %% 0 = %d\n", SAFE_MODULO(x, z));

    return 0;
}