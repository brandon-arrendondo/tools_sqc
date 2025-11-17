/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: global_storage.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to use global
 * storage for a structure containing a flexible array member. Global storage
 * duration is prohibited for flexible array structures.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* VIOLATION: Global storage for flexible array structure */
struct flex_array_struct global_flex;

int main(void) {
    global_flex.num = 5;

    /* VIOLATION: Accessing flexible array member in global storage */
    for (size_t i = 0; i < global_flex.num; i++) {
        global_flex.data[i] = (int)(i * i);  /* Undefined behavior */
    }

    printf("Global flex array first element: %d\n", global_flex.data[0]);

    return 0;
}