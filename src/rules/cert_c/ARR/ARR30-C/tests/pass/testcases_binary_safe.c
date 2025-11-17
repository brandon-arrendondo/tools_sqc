/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Binary search maintains proper bounds throughout execution
 */

#include <stdio.h>

int binary_search(int *arr, size_t size, int target) {
    size_t left = 0;
    size_t right = size;

    while (left < right) {
        size_t mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return (int)mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return -1;
}

int main(void) {
    int sorted_arr[] = {2, 5, 8, 12, 16, 23, 38, 45, 67, 78};
    size_t size = sizeof(sorted_arr) / sizeof(sorted_arr[0]);

    int result = binary_search(sorted_arr, size, 23);
    if (result != -1) {
        printf("Found at index %d\n", result);
    } else {
        printf("Not found\n");
    }

    return 0;
}