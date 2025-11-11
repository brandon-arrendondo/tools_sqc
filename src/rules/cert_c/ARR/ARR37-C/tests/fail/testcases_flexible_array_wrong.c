/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on non-flexible-array struct member
 */

#include <stdlib.h>

struct container {
    int count;
    int first_item;  // Not a flexible array member, just a regular int
};

void access_items(struct container *c) {
    int *ptr = &c->first_item;

    // Incorrectly treating single member as array
    for (int i = 0; i < 5; i++) {
        ptr[i] = i;  // Line 19 - VIOLATION
    }
}

int main(void) {
    struct container *c = malloc(sizeof(struct container));
    if (c) {
        access_items(c);
        free(c);
    }
    return 0;
}
