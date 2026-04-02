/*
 * Rule: MSC12-C
 * Status: PASS - While loop with actual body
 */

#include <stdio.h>

void f(int *arr, int n) {
    int i = 0;
    while (i < n) {
        printf("%d\n", arr[i]);
        i++;
    }
}
