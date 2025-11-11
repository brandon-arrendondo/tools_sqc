/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using char/byte pointer for byte-based arithmetic
 */

#include <stdlib.h>
#include <stddef.h>

struct container {
    int header;
    double data[50];
};

void byte_level_access(void) {
    struct container c;
    size_t offset = offsetof(struct container, data);

    // Use unsigned char* for byte arithmetic - COMPLIANT
    unsigned char *byte_ptr = (unsigned char *)&c;
    unsigned char *data_ptr = byte_ptr + offset;

    // Now can safely cast back if needed
    double *typed_ptr = (double *)data_ptr;
    *typed_ptr = 1.5;
}

int main(void) {
    byte_level_access();
    return 0;
}
