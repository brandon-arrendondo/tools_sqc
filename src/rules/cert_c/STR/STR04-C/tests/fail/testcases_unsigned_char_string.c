/*
 * Rule: STR04-C
 * Source: testcases
 * Status: FAIL - Should trigger STR04-C violation
 * Description: unsigned char array used with string literal
 */

#include <string.h>

void unsigned_char_strings(void) {
    unsigned char msg[] = "Warning";    /* Violation: unsigned char with string */
    unsigned char path[] = "/tmp/out";  /* Violation: unsigned char with string */
}
