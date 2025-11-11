/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>

void process_data(int *data) {
    size_t count = sizeof(data) / sizeof(int);

    for (size_t i = 0; i < count; i++) {
        data[i] = i;
    }
}

int main() {
    int buffer[100];

    process_data(buffer);

    return 0;
}