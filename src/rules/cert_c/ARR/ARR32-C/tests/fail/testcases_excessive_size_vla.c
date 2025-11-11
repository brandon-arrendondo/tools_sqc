/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void create_huge_array(void) {
    size_t huge_size = 10000000;  // 10 million elements

    int array[huge_size];

    for (size_t i = 0; i < huge_size; i++) {
        array[i] = i;
    }

    printf("Created huge array\n");
}

int main() {
    create_huge_array();
    return 0;
}