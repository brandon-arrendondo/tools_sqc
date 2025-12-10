/*
 * Rule: INT15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT15-C violation
 * Description: Microsoft-specific workaround for uintmax_t printf
 */

#include <stdio.h>
#include <inttypes.h>

typedef unsigned long long mytypedef_t;

void compliant_microsoft(void) {
    mytypedef_t x = 42;

    /* Compliant: using uintmax_t even with platform-specific format */
#ifdef _MSC_VER
    printf("%llu", (uintmax_t) x);
#else
    printf("%ju", (uintmax_t) x);
#endif
}
