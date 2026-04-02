/*
 * Rule: MSC13-C
 * Status: PASS - Variable used as array subscript
 */

#include <stdio.h>

void f(void) {
    int arr[10] = {0};
    int idx = 3;
    printf("%d\n", arr[idx]);
}
