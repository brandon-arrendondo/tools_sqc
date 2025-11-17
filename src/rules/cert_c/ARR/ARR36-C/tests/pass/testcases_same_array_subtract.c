/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Subtracting pointers within the same array
 */

#include <stddef.h>
#include <stdio.h>

void same_array_subtract(void) {
    int numbers[20] = {0};
    int *start = &numbers[5];
    int *end = &numbers[15];

    // Subtract pointers within same array - COMPLIANT
    ptrdiff_t diff = end - start;
    printf("Distance: %td\n", diff);
}

int main(void) {
    same_array_subtract();
    return 0;
}
