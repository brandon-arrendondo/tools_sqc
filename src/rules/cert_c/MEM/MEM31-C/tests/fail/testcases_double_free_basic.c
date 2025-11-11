/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(10 * sizeof(int));

    if (ptr != NULL) {
        // Use the memory
        for (int i = 0; i < 10; i++) {
            ptr[i] = i;
        }

        // First free
        free(ptr);

        // Double free - violation of MEM31-C
        free(ptr);

        printf("Memory freed twice\n");
    }

    return 0;
}