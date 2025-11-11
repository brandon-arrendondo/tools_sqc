/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: macro_uninitialized.c
 */

#include <stdio.h>

#define PROCESS_VALUE(x) printf("Processing: %d\n", (x))

/* NON-COMPLIANT: Macro with uninitialized argument */
void unsafe_macro_usage(void) {
    int value;  /* Uninitialized */

    PROCESS_VALUE(value);  /* Passing uninitialized value to macro */
}

int main(void) {
    unsafe_macro_usage();
    return 0;
}