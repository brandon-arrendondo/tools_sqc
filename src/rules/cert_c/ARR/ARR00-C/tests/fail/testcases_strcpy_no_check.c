/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

int main() {
    char small[5];
    char *input = "This is a very long string that will overflow";

    strcpy(small, input);

    return 0;
}