/*
 * Rule: MSC13-C
 * Status: PASS - Variable is used in printf
 */

#include <stdio.h>

void f(void) {
    int x = 42;
    printf("%d\n", x);
}
