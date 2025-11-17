/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Subtracting using pointer one past the end of array
 */

#include <stddef.h>
#include <stdio.h>

enum { SIZE = 32 };

void one_past_end(void) {
    int nums[SIZE];
    int *next_num_ptr = &nums[10];

    // Subtract from pointer one past end - COMPLIANT
    size_t free_elements = &(nums[SIZE]) - next_num_ptr;
    printf("Free elements: %zu\n", free_elements);
}

int main(void) {
    one_past_end();
    return 0;
}
