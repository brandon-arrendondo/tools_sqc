/*
 * Rule: MSC12-C
 * Status: PASS - If body has actual statements
 */

#include <stdio.h>

void f(int x) {
    if (x > 0) {
        printf("positive\n");
    }
}
