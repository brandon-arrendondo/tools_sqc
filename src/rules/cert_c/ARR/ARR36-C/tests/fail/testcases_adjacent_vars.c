/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Assuming adjacent variables and subtracting their addresses
 */

#include <stddef.h>

enum { SIZE = 32 };

void func(void) {
    int nums[SIZE];
    int end;
    int *next_num_ptr = nums;
    size_t free_elements;

    // Assumes nums and end are adjacent - VIOLATION
    free_elements = &end - next_num_ptr;  // Line 18 - VIOLATION
}

int main(void) {
    func();
    return 0;
}
