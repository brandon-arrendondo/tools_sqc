/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Proper use of flexible array member with pointer arithmetic
 */

#include <stdlib.h>
#include <stdio.h>

struct flex_array {
    size_t length;
    int data[];  // Flexible array member
};

struct flex_array *create_flex(size_t n) {
    struct flex_array *arr = malloc(sizeof(struct flex_array) + n * sizeof(int));

    if (arr) {
        arr->length = n;
        int *ptr = arr->data;

        // Pointer arithmetic on flexible array member - COMPLIANT
        for (size_t i = 0; i < n; i++) {
            *(ptr + i) = (int)i;
        }
    }
    return arr;
}

int main(void) {
    struct flex_array *arr = create_flex(10);
    if (arr) {
        printf("arr->data[5] = %d\n", arr->data[5]);
        free(arr);
    }
    return 0;
}
