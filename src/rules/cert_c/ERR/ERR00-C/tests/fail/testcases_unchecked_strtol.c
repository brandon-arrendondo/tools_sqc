/*
 * Rule: ERR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR00-C violation
 * Description: strtol called without checking errno or endptr
 */

#include <stdlib.h>

long parse_number(const char *str) {
    long val = strtol(str, NULL, 10);  /* Violation: no error check */
    return val;
}
