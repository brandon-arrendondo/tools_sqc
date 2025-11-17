/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

int main() {
    int source[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int dest[5];

    memcpy(dest, source, sizeof(source));

    return 0;
}