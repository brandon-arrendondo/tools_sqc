/*
 * Rule: STR04-C
 * Source: testcases
 * Status: FAIL - Should trigger STR04-C violation
 * Description: signed char array used with string literal
 */

#include <string.h>

void signed_char_strings(void) {
    signed char name[] = "John";        /* Violation: signed char with string */
    signed char greeting[] = "Hello";   /* Violation: signed char with string */

    size_t len = strlen((char *)name);
}
