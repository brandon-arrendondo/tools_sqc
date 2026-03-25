/*
 * Rule: MEM10-C
 * Source: testcases
 * Status: PASS - Correct sizeof usage
 */

#include <stdlib.h>
#include <string.h>

/* sizeof(*pointer) — correct */
void alloc_correct(void) {
    int *arr = (int *)malloc(10 * sizeof(*arr));
    if (arr) free(arr);
}

/* sizeof(type) — correct */
void alloc_type(void) {
    int *p = (int *)malloc(sizeof(int));
    if (p) free(p);
}

/* sizeof on local array — correct */
void clear_array(void) {
    int buf[100];
    memset(buf, 0, sizeof(buf));
}
