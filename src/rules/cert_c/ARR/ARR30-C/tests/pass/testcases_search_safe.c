/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Array search validates index before access
 */

#include <stdio.h>

int find_value(int *arr, size_t size, int target) {
    for (size_t i = 0; i < size; i++) {
        if (arr[i] == target) {
            return (int)i;  // Found at index i
        }
    }
    return -1;  // Not found
}

int main(void) {
    int numbers[] = {15, 23, 8, 42, 16, 35};
    size_t arr_size = sizeof(numbers) / sizeof(numbers[0]);
    int target = 42;

    int index = find_value(numbers, arr_size, target);

    if (index >= 0 && index < (int)arr_size) {
        printf("Found %d at index %d\n", target, index);
    } else {
        printf("Value %d not found\n", target);
    }

    return 0;
}