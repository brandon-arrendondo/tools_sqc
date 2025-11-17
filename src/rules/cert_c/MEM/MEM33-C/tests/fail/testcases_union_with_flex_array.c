/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: union_with_flex_array.c
 *
 * This case demonstrates a violation of MEM33-C by using a union that
 * contains a structure with a flexible array member. Unions with
 * flexible array structures are problematic and violate the rule.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

union flex_union {
    struct flex_array_struct flex;  /* VIOLATION: Flexible array in union */
    int simple_int;
    char simple_char;
};

int main(void) {
    /* VIOLATION: Automatic storage for union containing flexible array */
    union flex_union u;

    u.flex.num = 3;

    /* VIOLATION: Accessing flexible array member through union */
    u.flex.data[0] = 10;  /* Undefined behavior */
    u.flex.data[1] = 20;  /* Undefined behavior */
    u.flex.data[2] = 30;  /* Undefined behavior */

    printf("Union flex num: %zu\n", u.flex.num);
    printf("Union flex data: %d, %d, %d\n",
           u.flex.data[0], u.flex.data[1], u.flex.data[2]);

    return 0;
}