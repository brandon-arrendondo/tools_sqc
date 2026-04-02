/*
 * Rule: MSC38-C
 * Status: PASS - Normal use of errno (not suppressing the macro)
 */

#include <errno.h>
#include <stdio.h>

void f(void) {
    errno = 0;
    if (errno != 0) {
        printf("Error: %d\n", errno);
    }
}
