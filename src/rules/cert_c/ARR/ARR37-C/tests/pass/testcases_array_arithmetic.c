/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Pointer arithmetic on actual array
 */

#include <stdio.h>

void array_operations(void) {
    int numbers[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int *ptr = numbers;

    // Pointer arithmetic on array - COMPLIANT
    for (int i = 0; i < 10; i++) {
        printf("%d ", *(ptr + i));
    }
    printf("\n");

    // Increment pointer within array - COMPLIANT
    ptr++;
    printf("Second element: %d\n", *ptr);
}

int main(void) {
    array_operations();
    return 0;
}
