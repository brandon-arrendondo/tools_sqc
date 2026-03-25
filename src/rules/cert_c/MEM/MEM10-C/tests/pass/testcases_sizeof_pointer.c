/*
 * Rule: MEM10-C
 * Source: testcases
 * Status: PASS - Known limitation: sizeof(pointer) misuse not detected
 * TODO: Move to fail/ when sizeof(ptr) vs sizeof(*ptr) check is implemented (see PLAN.md)
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
