/*
 * Rule: API07-C
 * Source: testcases
 * Status: FAIL - Free of pointer not at allocation start
 */

#include <stdlib.h>
#include <string.h>

/* Pointer increment then free */
void free_after_increment(void) {
    char *p = (char *)malloc(100);
    if (p == NULL) return;
    p++;
    free(p);
}

/* Pointer arithmetic with += then free */
void free_after_compound_add(void) {
    char *buf = (char *)malloc(256);
    if (buf == NULL) return;
    buf += 10;
    free(buf);
}

/* Pre-decrement then free */
void free_after_predecrement(void) {
    int *arr = (int *)malloc(40 * sizeof(int));
    if (arr == NULL) return;
    ++arr;
    free(arr);
}
