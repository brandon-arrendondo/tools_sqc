/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: calloc() return value is not checked for NULL before use
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *array = calloc(100, sizeof(int)); // VIOLATION: No NULL check

    // Direct use without checking if allocation succeeded
    array[0] = 42; // Potential NULL pointer dereference
    array[99] = 100;

    printf("First element: %d\n", array[0]);
    printf("Last element: %d\n", array[99]);

    free(array);
    return 0;
}