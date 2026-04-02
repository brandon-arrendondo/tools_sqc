/*
 * Rule: INT31-C
 * Status: PASS - Widening cast is always safe
 */

#include <stdio.h>

void f(int i) {
    long l = i;       /* Widening: int to long — always safe */
    printf("%ld\n", l);
}

void g(char c) {
    int i = c;        /* Widening: char to int — always safe */
    printf("%d\n", i);
}
