/*
 * Rule: MSC12-C
 * Status: FAIL - Empty else body
 */

#include <stdio.h>

void f(int x) {
    if (x > 0) {
        printf("positive\n");
    } else {
        /* empty else — VIOLATION */
    }
}
