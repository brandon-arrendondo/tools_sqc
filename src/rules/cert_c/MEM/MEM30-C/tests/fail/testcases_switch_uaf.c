/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Switch statement frees memory but fallthrough accesses it
 */

#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 33;
    int mode = 1;

    switch (mode) {
        case 1:
            free(ptr);
            // Missing break - fallthrough
        case 2:
            // BUG: Access freed memory from case 1
            printf("Value: %d\n", *ptr);
            break;
        default:
            break;
    }

    return 0;
}