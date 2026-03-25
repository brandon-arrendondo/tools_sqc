/*
 * Rule: STR03-C
 * Source: testcases
 * Status: PASS - Safe integer-to-char conversion
 */

#include <stdlib.h>

/* Regular atoi to int — no truncation issue */
int get_int_from_string(const char *str) {
    return atoi(str);
}

/* Regular strtol to long — no truncation */
long get_long_from_string(const char *str) {
    return strtol(str, NULL, 10);
}
