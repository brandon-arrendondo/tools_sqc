/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: automatic_storage.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to use automatic
 * storage (stack allocation) for a structure containing a flexible array member.
 * The C standard requires that structures with flexible array members have
 * dynamic storage duration.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    /* VIOLATION: Attempting to use automatic storage for flexible array structure */
    struct flex_array_struct local_struct;  /* This is invalid */

    local_struct.num = 5;
    /* Accessing local_struct.data results in undefined behavior */

    return 0;
}