/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int data[100];

    for (int i = 1; i <= 100; i++) {
        data[i] = i * 2;
    }

    return 0;
}