/*
 * Rule: FIO47-C
 * Source: testcases
 * Status: FAIL - Invalid format string patterns
 */

#include <stdio.h>

/* Wrong length modifier for type */
void wrong_length_modifier(void) {
    long val = 100L;
    printf("%hd\n", val);
}

/* Mismatched argument types */
void type_mismatch(void) {
    int i = 42;
    printf("%s\n", i);
}

/* Extra format specifier, missing argument */
void too_few_args(void) {
    printf("%d %d\n", 42);
}
