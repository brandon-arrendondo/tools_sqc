/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Wrapped multiplication allocating struct array
 */

#include <stdlib.h>

typedef struct {
    int id;
    char name[64];
    double value;
} record_t;

void allocate_records(unsigned int count) {
    // Multiplication may wrap
    record_t *records = malloc(count * sizeof(record_t));  // Line 17 - VIOLATION

    if (records) {
        free(records);
    }
}

int main(void) {
    allocate_records(UINT_MAX / 2);  // Will wrap
    return 0;
}
