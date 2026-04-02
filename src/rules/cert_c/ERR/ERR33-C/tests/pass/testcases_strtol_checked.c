/*
 * Rule: ERR33-C
 * Status: PASS - strtol with proper error checking
 */

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>

void f(const char *str) {
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    if (errno != 0 || endptr == str) {
        fprintf(stderr, "Conversion error\n");
        return;
    }
    printf("Value: %ld\n", val);
}
