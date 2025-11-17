/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>

void process_range(int *start, int *end) {
    while (start < end) {
        *start = *start * 3;
        start++;
    }
}

void reverse_array(int arr[], size_t length) {
    int *left = arr;
    int *right = arr + length - 1;

    while (left < right) {
        int temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
}

int main() {
    int data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    size_t data_length = sizeof(data) / sizeof(data[0]);

    process_range(data, data + data_length);

    reverse_array(data, data_length);

    for (size_t i = 0; i < data_length; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    return 0;
}