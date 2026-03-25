/* Rule: EXP46-C
 * Source: testcases
 * Status: PASS - Correct use of logical operators with Boolean operands
 */

#include <unistd.h>

/* Case 1: Logical AND with equality comparisons (correct) */
void test_logical_and(void) {
    int x = 5, y = 10;
    if (x == 5 && y == 10) {
        /* correct */
    }
}

/* Case 2: Logical OR with relational comparisons (correct) */
void test_logical_or(void) {
    int a = 3, b = 7;
    if (a > 0 || b < 100) {
        /* correct */
    }
}

/* Case 3: Bitwise AND on non-Boolean integer values (acceptable) */
void test_bitwise_on_integers(void) {
    unsigned int flags = 0xFF;
    unsigned int mask = 0x0F;
    if ((flags & mask) == 0x0F) {
        /* correct: bitwise on non-Boolean values */
    }
}

/* Case 4: Logical operators with function calls (correct) */
void test_func_call_logical(void) {
    if (getuid() == 0 && getgid() == 0) {
        /* correct */
    }
}
