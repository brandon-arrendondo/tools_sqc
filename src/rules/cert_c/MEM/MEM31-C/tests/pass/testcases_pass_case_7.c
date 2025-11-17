/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(100 * sizeof(int));
    if (ptr) {
        free(ptr);
        ptr = NULL;
    }
    return 0;
}
