/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using element index for struct array
 */

#include <stdlib.h>

struct record {
    int id;
    char name[50];
    double value;
};

void struct_array_access(void) {
    struct record *records = malloc(10 * sizeof(struct record));

    if (records) {
        // Use element index (3), not sizeof - COMPLIANT
        struct record *third = records + 3;
        third->id = 3;

        // Or use array notation - COMPLIANT
        records[5].id = 5;

        free(records);
    }
}

int main(void) {
    struct_array_access();
    return 0;
}
