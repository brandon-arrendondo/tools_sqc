/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: nested_struct_violation.c
 *
 * This case demonstrates a violation of MEM33-C by improperly handling
 * a structure that contains another structure with a flexible array member.
 * Nested flexible array structures require careful memory management.
 */

#include <stdio.h>
#include <stdlib.h>

struct inner_flex {
    size_t count;
    char data[];  /* Flexible array member */
};

struct outer_struct {
    int id;
    struct inner_flex flex_member;  /* VIOLATION: Embedding flexible array struct */
};

int main(void) {
    /* VIOLATION: Automatic storage for struct containing flexible array */
    struct outer_struct outer;

    outer.id = 42;
    outer.flex_member.count = 3;

    /* VIOLATION: Accessing flexible array in embedded structure */
    outer.flex_member.data[0] = 'A';  /* Undefined behavior */
    outer.flex_member.data[1] = 'B';  /* Undefined behavior */
    outer.flex_member.data[2] = 'C';  /* Undefined behavior */

    printf("Outer ID: %d\n", outer.id);
    printf("Inner count: %zu\n", outer.flex_member.count);
    printf("Inner data: %c%c%c\n",
           outer.flex_member.data[0],
           outer.flex_member.data[1],
           outer.flex_member.data[2]);

    return 0;
}