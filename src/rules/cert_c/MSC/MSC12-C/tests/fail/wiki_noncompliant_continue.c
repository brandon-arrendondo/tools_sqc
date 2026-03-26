/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC12-C violation
 * Pattern: Meaningless continue at end of loop
 */

#include <stdio.h>

void func(void) {
    for (int i = 0; i < 10; ++i) {
        printf("i is %d", i);
        continue;  /* Noncompliant: loop continues anyway */
    }
}
