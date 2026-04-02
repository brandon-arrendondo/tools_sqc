/*
 * Rule: ERR33-C
 * Status: FAIL - strtol return value not checked for errors
 */

#include <stdlib.h>

void f(const char *str) {
    long val = strtol(str, NULL, 10);  /* VIOLATION: no error check */
}
