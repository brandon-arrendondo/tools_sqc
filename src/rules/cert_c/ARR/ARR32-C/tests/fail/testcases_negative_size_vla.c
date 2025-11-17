/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void create_negative_size_array(void) {
    int size = -10;

    int array[size];

    printf("Created array with negative size\n");
}

int main() {
    create_negative_size_array();
    return 0;
}