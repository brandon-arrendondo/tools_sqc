/*
 * Rule: INT15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT15-C violation
 * Description: Using uintmax_t for formatted I/O on programmer-defined types
 */

#include <stdio.h>
#include <inttypes.h>

typedef unsigned long long mytypedef_t;

void compliant(void) {
    mytypedef_t x = 42;
    /* Compliant: casting to uintmax_t with %ju format */
    printf("%ju", (uintmax_t) x);
}
