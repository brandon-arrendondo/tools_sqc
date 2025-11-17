/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>

typedef int int_array[];

void process_typedef_array(int_array arr) {
    size_t count = sizeof(arr) / sizeof(arr[0]);

    for (size_t i = 0; i < count; i++) {
        arr[i] = i * 2;
    }
}

int main() {
    int data[25];

    process_typedef_array(data);

    return 0;
}