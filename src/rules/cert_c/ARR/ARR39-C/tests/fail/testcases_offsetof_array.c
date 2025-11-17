/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using offsetof with array element pointer
 */

#include <stddef.h>
#include <string.h>

struct container {
    int header;
    double data[50];
};

void offsetof_array_access(void) {
    struct container c;
    size_t offset = offsetof(struct container, data);

    // Using offsetof with typed pointer
    double *data_ptr = (double *)(&c) + offset;  // Line 19 - VIOLATION
    *data_ptr = 1.5;
}

int main(void) {
    offsetof_array_access();
    return 0;
}
