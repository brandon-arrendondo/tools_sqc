/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

int main() {
    char buffer[20] = "Hello ";
    char *addition = "This is a very long string to concatenate";

    strcat(buffer, addition);

    return 0;
}