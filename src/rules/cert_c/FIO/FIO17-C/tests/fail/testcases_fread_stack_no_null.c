/*
 * Rule: FIO17-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO17-C violation
 *
 * fread() on stack buffer without null terminator
 */

#include <stdio.h>

void read_no_null_terminator(FILE *fp) {
    char buffer[256];
    size_t count = fread(buffer, 1, sizeof(buffer), fp);
    /* VIOLATION: no null terminator set after fread */
    printf("%s\n", buffer);
}
