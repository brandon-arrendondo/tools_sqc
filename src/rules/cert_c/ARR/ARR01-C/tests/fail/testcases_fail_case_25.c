/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>

void wrong_sizeof_usage(int arr[]) {
    size_t size = sizeof(arr) / sizeof(arr[0]);
    
    for (size_t i = 0; i < size; i++) {
        arr[i] = 0;
    }
}

int main() {
    int data[50];
    wrong_sizeof_usage(data);
    return 0;
}
