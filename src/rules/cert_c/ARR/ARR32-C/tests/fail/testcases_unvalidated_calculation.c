/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void create_calculated_array(size_t rows, size_t cols) {
    size_t total = rows * cols;  // No overflow check

    int array[total];

    printf("Created calculated array\n");
}

int main() {
    create_calculated_array(100000, 100000);  // Huge multiplication
    return 0;
}