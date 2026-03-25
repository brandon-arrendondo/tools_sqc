/* Rule: EXP46-C
 * Source: testcases
 * Status: FAIL - Bitwise operator used with Boolean-like operands
 */

#include <unistd.h>

/* Case 1: Bitwise AND with equality comparisons */
void test_bitwise_and_equality(void) {
    int x = 5, y = 10;
    if (x == 5 & y == 10) {
        /* should use && */
    }
}

/* Case 2: Bitwise OR with relational comparisons */
void test_bitwise_or_relational(void) {
    int a = 3, b = 7;
    if (a > 0 | b < 100) {
        /* should use || */
    }
}

/* Case 3: Bitwise XOR with equality check */
void test_bitwise_xor_equality(void) {
    int status = 0;
    if (status == 0 ^ status != -1) {
        /* should use != for XOR logic */
    }
}

/* Case 4: Bitwise AND with function call equality checks */
void test_func_call_equality(void) {
    if (getuid() == 0 & getgid() == 0) {
        /* should use && */
    }
}
