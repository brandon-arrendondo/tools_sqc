/*
 * Rule: STR37-C
 * Source: testcases
 * Status: FAIL - Arguments to character-handling functions must be representable as unsigned char
 */

#include <ctype.h>

/* Passing plain char to isalpha (may be negative) */
int check_alpha(char c) {
    return isalpha(c);
}

/* Passing plain char to toupper */
int to_upper(char c) {
    return toupper(c);
}

/* Passing plain char to isdigit */
int check_digit(char c) {
    return isdigit(c);
}

/* Passing plain char to isspace */
int check_space(char c) {
    return isspace(c);
}
