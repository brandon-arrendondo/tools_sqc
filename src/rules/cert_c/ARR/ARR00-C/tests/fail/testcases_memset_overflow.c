/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

int main() {
    int numbers[5];

    memset(numbers, 0, 100);

    return 0;
}