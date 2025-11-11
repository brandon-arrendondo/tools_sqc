/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: static_storage.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to use static
 * storage for a structure containing a flexible array member. Static storage
 * duration is also prohibited for flexible array structures.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* VIOLATION: Static storage for flexible array structure */
static struct flex_array_struct static_flex;

int main(void) {
    static_flex.num = 3;

    /* VIOLATION: Accessing flexible array member in static storage */
    static_flex.data[0] = 10;  /* Undefined behavior */
    static_flex.data[1] = 20;  /* Undefined behavior */
    static_flex.data[2] = 30;  /* Undefined behavior */

    printf("Static flex array: %d, %d, %d\n",
           static_flex.data[0], static_flex.data[1], static_flex.data[2]);

    return 0;
}