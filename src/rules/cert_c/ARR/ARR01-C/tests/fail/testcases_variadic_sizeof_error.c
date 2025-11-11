/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>
#include <stdarg.h>

void process_arrays(int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        int *arr = va_arg(args, int*);
        size_t size = sizeof(arr);
        printf("Array %d size: %zu\n", i, size);
    }

    va_end(args);
}

int main() {
    int arr1[10], arr2[20];
    process_arrays(2, arr1, arr2);
    return 0;
}