/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: multiple_flex_members.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to declare
 * a structure with multiple flexible array members, which is invalid
 * according to the C standard. Only one flexible array member is allowed
 * and it must be the last member.
 */

#include <stdio.h>
#include <stdlib.h>

/* VIOLATION: Structure with multiple flexible array members (invalid syntax) */
struct invalid_multi_flex {
    size_t num1;
    int data1[];    /* First flexible array member */
    size_t num2;    /* VIOLATION: Member after flexible array */
    char data2[];   /* VIOLATION: Second flexible array member */
};

/* VIOLATION: Flexible array member not at the end */
struct invalid_flex_position {
    size_t num;
    int data[];     /* VIOLATION: Flexible array not at end */
    char extra;     /* VIOLATION: Member after flexible array */
};

int main(void) {
    /* These structures cannot be properly used due to invalid declaration */

    /* Attempting to use the invalid structures would result in compilation errors
     * or undefined behavior. This demonstrates why the C standard requires
     * exactly one flexible array member at the end of the structure.
     */

    printf("This code demonstrates invalid flexible array member usage\n");
    printf("Multiple flexible arrays or non-terminal position violates C standard\n");

    return 0;
}