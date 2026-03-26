/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC12-C violation
 * Pattern: Conditions with side effects (getc advances stream)
 */

#include <stdio.h>

void readMoreInput(void);

void func(FILE *fp) {
    /* Compliant: getc() has side effects, so duplicate text is not redundant */
    if (getc(fp) == ':')
        readMoreInput();
    else if (getc(fp) == ':')
        readMoreInput();
    else if (getc(fp) == ':')
        readMoreInput();
}
