/*
 * Rule: INT15-C
 * Source: wiki
 * Status: FAIL - Should trigger INT15-C violation
 * Description: Using scanf with programmer-defined type may truncate
 */

#include <stdio.h>

typedef unsigned long long mytypedef_t;

void noncompliant(void) {
    mytypedef_t x;
    /* Violation: scanf directly to programmer-defined type */
    if (scanf("%llu", &x) != 1) {
        /* Handle error */
    }
}
