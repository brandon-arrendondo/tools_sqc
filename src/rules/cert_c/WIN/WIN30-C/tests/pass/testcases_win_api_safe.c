/*
 * Rule: WIN30-C
 * Source: testcases
 * Status: PASS - Windows API with proper security attributes
 */

/* Non-Windows code — no violations expected */
#include <stdio.h>

void portable_code(void) {
    FILE *f = fopen("test.txt", "r");
    if (f) fclose(f);
}
