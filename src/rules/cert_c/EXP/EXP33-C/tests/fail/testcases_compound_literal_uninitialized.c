/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: compound_literal_uninitialized.c
 */

#include <stdio.h>

struct Point { int x, y; };

/* NON-COMPLIANT: Compound literal with uninitialized values */
void unsafe_compound_literal(void) {
    int a, b;  /* Uninitialized */

    struct Point p = (struct Point){a, b};  /* Using uninitialized values */
    printf("Point: (%d, %d)\n", p.x, p.y);
}

int main(void) {
    unsafe_compound_literal();
    return 0;
}