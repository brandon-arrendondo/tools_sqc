/*
 * Rule: INT00-C
 * Source: testcases
 * Status: FAIL - printf family format specifier mismatches
 */

#include <stdio.h>

/* %ld with int variable */
void printf_ld_int(void) {
    int x = 42;
    printf("value: %ld\n", x);
}

/* %lld with int variable */
void printf_lld_int(void) {
    int x = 42;
    printf("value: %lld\n", x);
}

/* fprintf with %lu for int */
void fprintf_lu_int(void) {
    int x = 42;
    fprintf(stderr, "value: %lu\n", x);
}

/* sprintf with %llu for int */
void sprintf_llu_int(void) {
    char buf[64];
    int x = 42;
    sprintf(buf, "value: %llu\n", x);
}

/* snprintf with mismatched specifier */
void snprintf_mismatch(void) {
    char buf[64];
    int x = 42;
    snprintf(buf, sizeof(buf), "value: %ld\n", x);
}

/* %d with long variable */
void printf_d_long(void) {
    long x = 42;
    printf("value: %d\n", x);
}
