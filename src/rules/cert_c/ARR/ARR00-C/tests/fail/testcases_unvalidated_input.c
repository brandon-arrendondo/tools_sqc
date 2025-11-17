/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

int main() {
    int data[100];
    int count;

    printf("How many numbers? ");
    scanf("%d", &count);

    for (int i = 0; i < count; i++) {
        scanf("%d", &data[i]);
    }

    return 0;
}