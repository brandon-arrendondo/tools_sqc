/*
 * Rule: DCL03-C
 * Source: testcases
 * Status: FAIL - Runtime assert() used with constant expressions
 */

#include <assert.h>

/* assert with sizeof comparison */
void assert_sizeof(void) {
    assert(sizeof(int) == 4);
}

/* assert with numeric literal comparison */
void assert_numeric_literal(void) {
    assert(8 > 4);
}

/* assert with sizeof arithmetic */
struct Packet {
    int header;
    int payload;
};
void assert_sizeof_struct(void) {
    assert(sizeof(struct Packet) >= sizeof(int) + sizeof(int));
}

/* assert with character literal */
void assert_char_literal(void) {
    assert('A' == 65);
}

/* assert with compound constant expression */
void assert_compound_constant(void) {
    assert(sizeof(long) >= sizeof(int));
}
