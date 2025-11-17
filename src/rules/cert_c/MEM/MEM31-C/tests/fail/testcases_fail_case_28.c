/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(100 * sizeof(int));
    if (ptr) {
        free(ptr);
        free(ptr);  // Double free
    }
    return 0;
}
