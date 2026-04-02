/*
 * Rule: MSC13-C
 * Status: PASS - Variable is used in if condition
 */

#include <stdio.h>

void f(void) {
    int x = 42;
    if (x > 10) {
        printf("large\n");
    }
}
