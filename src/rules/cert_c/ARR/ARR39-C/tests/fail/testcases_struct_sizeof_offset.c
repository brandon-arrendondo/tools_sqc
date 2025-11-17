/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof(struct) as offset with struct pointer
 */

#include <stdlib.h>

struct record {
    int id;
    char name[50];
    double value;
};

void struct_offset(void) {
    struct record *records = malloc(10 * sizeof(struct record));

    if (records) {
        // Using sizeof(struct record) as index - double-scaling
        struct record *third = records + sizeof(struct record);  // Line 20 - VIOLATION
        third->id = 3;

        free(records);
    }
}

int main(void) {
    struct_offset();
    return 0;
}
