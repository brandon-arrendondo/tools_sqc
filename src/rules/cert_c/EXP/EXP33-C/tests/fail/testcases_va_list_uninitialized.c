/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: va_list_uninitialized.c
 */

#include <stdio.h>
#include <stdarg.h>

/* NON-COMPLIANT: va_list used without proper initialization */
void unsafe_variadic(int count, ...) {
    va_list args;  /* Uninitialized va_list */

    /* Using va_list without va_start */
    for (int i = 0; i < count; i++) {
        int val = va_arg(args, int);  /* Undefined behavior */
        printf("Value: %d\n", val);
    }
}

int main(void) {
    unsafe_variadic(3, 1, 2, 3);
    return 0;
}