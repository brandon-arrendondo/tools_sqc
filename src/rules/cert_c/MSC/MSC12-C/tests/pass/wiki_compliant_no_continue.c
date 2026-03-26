/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC12-C violation
 * Pattern: Loop without unnecessary continue
 */

#include <stdio.h>

void func(void) {
    for (int i = 0; i < 10; ++i) {
        printf("i is %d", i);  /* Compliant: no meaningless continue */
    }
}
