/*
 * Rule: MSC13-C
 * Status: PASS - Variable used in increment expression
 */

#include <stdio.h>

void f(void) {
    int x = 0;
    x++;
    printf("%d\n", x);
}
