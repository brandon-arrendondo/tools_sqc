/*
 * Rule: MSC38-C
 * Status: FAIL - Taking address of errno (may be a macro)
 */

#include <errno.h>

void f(void) {
    int *p = &(errno);  /* VIOLATION: treating macro as object */
}
