/*
 * Rule: PRE04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE04-C violation
 *
 * Custom header names that don't match standard C headers
 */

/* COMPLIANT: unique custom header names */
#include "mystring.h"
#include "app_math.h"
#include "project_time.h"

void process(void) {
    /* ... */
}
