/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int size = 0;
    int vla[size];

    vla[0] = 100;

    return 0;
}