/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>

void process_generic_array(void *data) {
    size_t size = sizeof(data);

    printf("Array size: %zu\n", size);
}

int main() {
    int numbers[100];
    double values[50];

    process_generic_array(numbers);
    process_generic_array(values);

    return 0;
}