/*
 * Rule: STR37-C
 * Source: testcases
 * Status: PASS - Proper unsigned char cast before character functions
 */

#include <ctype.h>

/* Cast to unsigned char before isalpha */
int check_alpha_safe(char c) {
    return isalpha((unsigned char)c);
}

/* Cast to unsigned char before toupper */
int to_upper_safe(char c) {
    return toupper((unsigned char)c);
}

/* No ctype calls — nothing to check */
int no_ctype(int x) {
    return x + 1;
}
