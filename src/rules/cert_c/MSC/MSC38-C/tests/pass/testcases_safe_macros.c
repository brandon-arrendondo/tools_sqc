/*
 * Rule: MSC38-C
 * Source: testcases
 * Status: PASS - Protected identifiers used correctly through standard headers
 */

#include <assert.h>
#include <errno.h>

/* Normal use of assert() macro is compliant */
void use_assert(int x) {
    assert(x > 0);
}

/* Using errno through the header is compliant */
int check_errno(void) {
    return errno;
}

/* Wrapper function instead of suppressing assert macro */
void assert_wrapper(int value) {
    assert(value);
}

/* Taking address of wrapper function, not of assert itself */
typedef void (*handler_type)(int);
void execute_handler(handler_type handler, int value) {
    handler(value);
}
void use_wrapper(int e) {
    execute_handler(&assert_wrapper, e < 0);
}

/* #undef of non-protected identifier is fine */
#define MY_MACRO 42
#undef MY_MACRO
