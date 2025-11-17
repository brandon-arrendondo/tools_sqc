/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Inline function performs array access without bounds checking
 */

#include <stdio.h>

static inline int get_element(int arr[], int index) {
    // No bounds checking in inline function
    return arr[index];
}

static inline void set_element(int arr[], int index, int value) {
    // No bounds checking in inline function
    arr[index] = value;
}

int main(void) {
    int data[4] = {100, 200, 300, 400};

    // Using inline functions with out-of-bounds indices
    printf("Element[6] = %d\n", get_element(data, 6));
    set_element(data, 8, 999);

    printf("Element[10] = %d\n", get_element(data, 10));

    return 0;
}