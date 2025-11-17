/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: goto_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Goto skipping initialization */
void unsafe_goto(int condition) {
    if (condition)
        goto skip_init;

    int value = 42;  /* This initialization may be skipped */

skip_init:
    printf("Value: %d\n", value);  /* May read uninitialized variable */
}

int main(void) {
    unsafe_goto(1);  /* Skip initialization */
    return 0;
}