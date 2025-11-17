/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: bitfield_uninitialized.c
 */

#include <stdio.h>

struct Flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int value : 6;
};

/* NON-COMPLIANT: Bitfield struct uninitialized */
void unsafe_bitfields(void) {
    struct Flags flags;  /* Uninitialized */

    if (flags.flag1) {  /* Reading uninitialized bitfield */
        printf("Flag1 is set\n");
    }

    printf("Value: %u\n", flags.value);  /* Reading uninitialized bitfield */
}

int main(void) {
    unsafe_bitfields();
    return 0;
}