/*
 * Rule: MEM10-C
 * Source: testcases
 * Status: FAIL - sizeof(pointer) misuse in allocation/memory functions
 */

#include <stdlib.h>

/* sizeof(pointer) instead of sizeof(*pointer) */
void alloc_wrong_size(void) {
    int *arr = (int *)malloc(sizeof(arr));
    free(arr);
}

/* sizeof(ptr) in memset */
void clear_wrong_size(int *buf) {
    memset(buf, 0, sizeof(buf));
}
