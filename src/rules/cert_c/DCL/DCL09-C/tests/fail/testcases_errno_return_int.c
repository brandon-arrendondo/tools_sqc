/*
 * Rule: DCL09-C
 * Source: testcases
 * Status: FAIL - Functions returning errno values should use errno_t return type
 */

#include <errno.h>

/* Returns errno constant with int return type */
int get_error_code(int input) {
    if (input < 0) {
        return EINVAL;
    }
    return 0;
}

/* Returns errno variable with int return type */
int check_status(void) {
    return errno;
}

/* Returns multiple errno constants */
int validate(int x, int y) {
    if (x < 0) return EINVAL;
    if (y < 0) return ERANGE;
    return 0;
}
