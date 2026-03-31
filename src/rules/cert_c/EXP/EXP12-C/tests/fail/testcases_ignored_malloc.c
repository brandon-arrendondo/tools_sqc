/*
 * Rule: EXP12-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP12-C violation
 * Description: Return values from allocation functions ignored
 */

#include <stdlib.h>

void ignore_allocation_returns(void) {
    malloc(100);     /* Violation: return value discarded */
    calloc(10, 20);  /* Violation: return value discarded */
    realloc(NULL, 50); /* Violation: return value discarded */
}
