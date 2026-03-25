/*
 * Rule: DCL03-C
 * Source: testcases
 * Status: PASS - Runtime assert with non-constant or static_assert used correctly
 */

#include <assert.h>

/* assert with runtime variable is compliant */
void assert_runtime_var(int x) {
    assert(x > 0);
}

/* assert with function call result is compliant */
int get_size(void);
void assert_function_call(void) {
    assert(get_size() > 0);
}

/* assert with pointer is compliant */
void assert_pointer(void *p) {
    assert(p != 0);
}

/* static_assert is compliant (not flagged) */
void uses_static_assert(void) {
    _Static_assert(sizeof(int) == 4, "int must be 4 bytes");
}

/* assert with variable comparison is compliant */
void assert_two_vars(int a, int b) {
    assert(a < b);
}
