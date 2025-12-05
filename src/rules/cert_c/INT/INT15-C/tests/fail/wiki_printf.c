/*
 * Rule: INT15-C
 * Source: wiki
 * Status: FAIL - Should trigger INT15-C violation
 * Description: Programmer-defined type cast to unsigned long long may truncate
 */

#include <stdio.h>

typedef unsigned long long mytypedef_t;

void noncompliant(void) {
    mytypedef_t x = 42;
    /* Violation: casting to unsigned long long instead of uintmax_t */
    printf("%llu", (unsigned long long) x);
}
