/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    char small_buffer[10];
    char *long_string = "This is a very long string that exceeds buffer capacity";

    sprintf(small_buffer, "%s", long_string);

    return 0;
}