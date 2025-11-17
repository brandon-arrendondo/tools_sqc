/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>

void print_array(int *arr, size_t count) {
    for (size_t i = 0; i < count; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int data[20];

    for (int i = 0; i < 20; i++) {
        data[i] = i * i;
    }

    size_t element_count = sizeof(data) / sizeof(data[0]);
    size_t total_bytes = sizeof(data);

    printf("Array has %zu elements\n", element_count);
    printf("Array occupies %zu bytes\n", total_bytes);

    print_array(data, element_count);

    return 0;
}