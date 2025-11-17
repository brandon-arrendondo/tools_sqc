/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Checks if pointer is NULL before accessing, preventing use-after-free
 */

#include <stdlib.h>
#include <stdio.h>

void safe_access(int *ptr) {
    if (ptr != NULL) {
        printf("Value: %d\n", *ptr);
    } else {
        printf("Pointer is NULL\n");
    }
}

int main() {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        return -1;
    }

    *ptr = 100;
    safe_access(ptr);

    free(ptr);
    ptr = NULL;

    safe_access(ptr);  // Safe - function checks for NULL
    return 0;
}