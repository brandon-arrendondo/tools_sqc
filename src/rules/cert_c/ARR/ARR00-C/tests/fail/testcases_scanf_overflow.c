/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    char name[10];

    printf("Enter your name: ");
    scanf("%s", name);

    printf("Hello %s\n", name);

    return 0;
}