/*
 * Rule: API02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API02-C violation
 *
 * const char * parameters are exempt (null-terminated strings)
 */

#include <stdio.h>

/* COMPLIANT: const char * uses null-terminator convention */
void log_message(const char *prefix, const char *message) {
    printf("[%s] %s\n", prefix, message);
}
