/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

void process_array(int arr[]) {
    size_t size = sizeof(arr) / sizeof(arr[0]);

    for (size_t i = 0; i < size; i++) {
        arr[i] = i;
    }
}

int main() {
    int numbers[100];

    process_array(numbers);

    return 0;
}