/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Heap buffer overflow through malloc'd memory access
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *buffer = malloc(5 * sizeof(int));

    if (buffer != NULL) {
        // Initialize allocated memory
        for (int i = 0; i < 5; i++) {
            buffer[i] = i * 10;
        }

        // Access beyond allocated bounds
        buffer[10] = 999;
        printf("buffer[10] = %d\n", buffer[10]);

        free(buffer);
    }

    return 0;
}