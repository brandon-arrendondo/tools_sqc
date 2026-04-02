/*
 * Rule: FIO17-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO17-C violation
 *
 * fread() with explicit null terminator after read
 */

#include <stdio.h>

void read_with_null_terminator(FILE *fp) {
    char buffer[256];
    size_t count = fread(buffer, 1, sizeof(buffer) - 1, fp);
    /* COMPLIANT: null terminator explicitly set */
    buffer[count] = '\0';
    printf("%s\n", buffer);
}
