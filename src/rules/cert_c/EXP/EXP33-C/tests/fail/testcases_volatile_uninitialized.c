/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: volatile_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Volatile variable uninitialized */
void unsafe_volatile(void) {
    volatile int data;  /* Uninitialized volatile */

    printf("Volatile data: %d\n", data);  /* Reading uninitialized volatile */
}

int main(void) {
    unsafe_volatile();
    return 0;
}