/*
 * Rule: MSC39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MSC39-C violation
 *
 * Function receives va_list by pointer and uses va_copy
 */

#include <stdarg.h>
#include <stdio.h>

/* COMPLIANT: va_list passed by pointer, uses va_copy */
void print_two(va_list *ap) {
    va_list local;
    va_copy(local, *ap);
    int a = va_arg(local, int);
    int b = va_arg(local, int);
    va_end(local);
    printf("%d %d\n", a, b);
}
