/*
 * Rule: MSC13-C
 * Status: PASS - Variable used in field expression read
 */

#include <stdio.h>

struct Point { int x; int y; };

void f(void) {
    struct Point p;
    p.x = 10;
    p.y = 20;
    printf("%d %d\n", p.x, p.y);
}
