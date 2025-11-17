/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    char buffer[50];

    printf("Enter input: ");
    gets(buffer);

    printf("You entered: %s\n", buffer);

    return 0;
}