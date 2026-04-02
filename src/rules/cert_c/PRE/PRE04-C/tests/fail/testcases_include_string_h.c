/*
 * Rule: PRE04-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE04-C violation
 *
 * Local include reusing standard header name "string.h"
 */

/* VIOLATION: reuses standard C header name */
#include "string.h"

void process_strings(void) {
    /* ... */
}
