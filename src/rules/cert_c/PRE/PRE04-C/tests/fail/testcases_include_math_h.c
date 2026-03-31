/*
 * Rule: PRE04-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE04-C violation
 *
 * Local include reusing standard header name "math.h"
 */

/* VIOLATION: reuses standard C header name */
#include "math.h"

double compute(double x) {
    return x * x;
}
