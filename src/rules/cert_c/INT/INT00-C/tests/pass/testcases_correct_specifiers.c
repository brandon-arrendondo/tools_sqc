/*
 * Rule: INT00-C
 * Source: testcases
 * Status: PASS - Correct format specifiers matching variable types
 */

#include <stdio.h>

/* %d with int (declared without initializer) */
void printf_d_int(int x) {
    printf("value: %d\n", x);
}

/* %c with char */
void printf_c_char(char c) {
    printf("value: %c\n", c);
}

/* %% is not a format specifier */
void printf_percent_literal(void) {
    printf("100%%\n");
}

/* No format arguments — nothing to check */
void printf_no_args(void) {
    printf("hello world\n");
}

/* Literal integer passed directly */
void printf_literal(void) {
    printf("value: %d\n", 42);
}
