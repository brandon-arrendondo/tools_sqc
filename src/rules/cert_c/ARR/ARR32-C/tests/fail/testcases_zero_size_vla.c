/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void create_zero_size_array(void) {
    size_t size = 0;

    int array[size];

    printf("Created zero-size array\n");
}

int main() {
    create_zero_size_array();
    return 0;
}