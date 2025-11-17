/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using total allocation bytes as pointer offset
 */

#include <stdlib.h>

void calloc_byte_offset(void) {
    size_t num_elements = 50;
    size_t elem_size = sizeof(int);

    int *array = (int *)calloc(num_elements, elem_size);

    if (array) {
        size_t total_bytes = num_elements * elem_size;
        // Using byte count as offset for int pointer
        int *end = array + total_bytes;  // Line 17 - VIOLATION

        free(array);
    }
}

int main(void) {
    calloc_byte_offset();
    return 0;
}
