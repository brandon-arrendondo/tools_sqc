/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Returns pointer to freed memory
 */

#include <stdlib.h>
#include <stdio.h>

int *create_data() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return NULL;
    }

    *ptr = 42;
    free(ptr);  // BUG: Free before returning

    return ptr;  // Returning freed pointer
}

int main() {
    int *data = create_data();
    if (data != NULL) {
        // BUG: Using returned freed pointer
        printf("Value: %d\n", *data);
    }

    return 0;
}