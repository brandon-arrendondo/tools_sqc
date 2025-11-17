/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

int main() {
    char buffer[10];

    strcpy(buffer, "This string is way too long for the buffer");

    return 0;
}