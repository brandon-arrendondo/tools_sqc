/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: enum_uninitialized.c
 */

#include <stdio.h>

enum Status { PENDING, ACTIVE, INACTIVE };

/* NON-COMPLIANT: Uninitialized enum usage */
void unsafe_enum_usage(void) {
    enum Status status;  /* Uninitialized */

    switch (status) {  /* Reading uninitialized enum */
        case PENDING:
            printf("Status is pending\n");
            break;
        case ACTIVE:
            printf("Status is active\n");
            break;
        case INACTIVE:
            printf("Status is inactive\n");
            break;
        default:
            printf("Unknown status\n");
    }
}

int main(void) {
    unsafe_enum_usage();
    return 0;
}