/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Structure with array member includes size field for bounds checking
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    int data[10];
    size_t count;  // Track actual number of elements
} SafeArray;

void add_element(SafeArray *arr, int value) {
    if (arr->count < sizeof(arr->data) / sizeof(arr->data[0])) {
        arr->data[arr->count] = value;
        arr->count++;
    } else {
        printf("Array is full, cannot add element\n");
    }
}

int main(void) {
    SafeArray safe_arr = {0};  // Initialize with zeros

    add_element(&safe_arr, 10);
    add_element(&safe_arr, 20);
    add_element(&safe_arr, 30);

    for (size_t i = 0; i < safe_arr.count; i++) {
        printf("data[%zu] = %d\n", i, safe_arr.data[i]);
    }

    return 0;
}