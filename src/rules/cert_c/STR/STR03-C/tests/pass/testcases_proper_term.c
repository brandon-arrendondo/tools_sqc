/*
 * Rule: STR03-C
 * Source: testcases
 * Status: PASS - Safe string patterns
 */

#include <string.h>

/* String literal — always null-terminated */
void literal_assign(void) {
    const char *s = "hello";
    (void)s;
}

/* Array with shorter string */
void array_shorter(void) {
    char buf[20] = "hello";
    (void)buf;
}

/* No string functions called */
void no_string_ops(void) {
    int x = 42;
    (void)x;
}
