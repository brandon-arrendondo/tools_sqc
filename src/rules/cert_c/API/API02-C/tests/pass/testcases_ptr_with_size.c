/*
 * Rule: API02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API02-C violation
 *
 * Pointer parameters paired with size arguments
 */

#include <stddef.h>

/* COMPLIANT: each writable pointer has a corresponding size */
void fill_buffer(int *buf, size_t buf_count, int pattern) {
    for (size_t i = 0; i < buf_count; i++) {
        buf[i] = pattern;
    }
}
