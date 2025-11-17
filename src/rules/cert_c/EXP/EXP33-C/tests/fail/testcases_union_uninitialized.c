/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: union_uninitialized.c
 */

#include <stdio.h>

union Data {
    int i;
    float f;
    char c[4];
};

/* NON-COMPLIANT: Reading uninitialized union member */
void unsafe_union_access(void) {
    union Data data;  /* Uninitialized */

    printf("Integer: %d\n", data.i);  /* Undefined behavior */
    printf("Float: %f\n", data.f);    /* Undefined behavior */
}

int main(void) {
    unsafe_union_access();
    return 0;
}