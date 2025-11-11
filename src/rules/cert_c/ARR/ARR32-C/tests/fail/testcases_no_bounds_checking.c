/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void create_array_no_check(size_t n) {
    int array[n];

    for (size_t i = 0; i < n; i++) {
        array[i] = i;
    }

    printf("Created array without bounds checking\n");
}

int main() {
    create_array_no_check(0);        // Zero size
    create_array_no_check(100000);   // Very large
    return 0;
}