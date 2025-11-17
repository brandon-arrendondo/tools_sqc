/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

/*
 * ARR00-C PASS Case: Proper bounds checking
 *
 * This test case demonstrates compliant code that validates array indices
 * before accessing array elements. This is fundamental to preventing
 * buffer overflow vulnerabilities.
 *
 * Key security features:
 * - Input validation before array access
 * - Clear bounds checking logic
 * - Graceful handling of invalid indices
 *
 * Vulnerability prevention:
 * - Buffer overflow attacks
 * - Out-of-bounds memory access
 * - Potential arbitrary code execution
 */

#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 10

int main() {
    int arr[ARRAY_SIZE];
    int index;

    printf("Enter an index (0-%d): ", ARRAY_SIZE - 1);
    scanf("%d", &index);

    // Critical security check: validate index before array access
    if (index >= 0 && index < ARRAY_SIZE) {
        arr[index] = 42;
        printf("Value at index %d set to %d\n", index, arr[index]);
    } else {
        // Secure error handling - no array access with invalid index
        printf("Index %d is out of bounds\n", index);
    }

    return 0;
}