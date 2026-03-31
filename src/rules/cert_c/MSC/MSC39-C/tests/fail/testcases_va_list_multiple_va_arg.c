/*
 * Rule: MSC39-C
 * Source: testcases
 * Status: FAIL - Should trigger MSC39-C violation
 *
 * Function receives va_list by value and calls va_arg() multiple times
 */

#include <stdarg.h>
#include <stdio.h>

/* VIOLATION: va_list by value with va_arg makes caller's va_list indeterminate */
void print_two(va_list ap) {
    int a = va_arg(ap, int);
    int b = va_arg(ap, int);
    printf("%d %d\n", a, b);
}
