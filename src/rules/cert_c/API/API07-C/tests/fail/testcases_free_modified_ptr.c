/*
 * Rule: API07-C
 * Source: testcases
 * Status: FAIL - free() on pointer modified after allocation
 */

#include <stdlib.h>
#include <string.h>

/* ptr++ then free — classic CWE-761 */
void free_after_increment(void) {
    char *buf = (char *)malloc(100);
    buf++;
    free(buf);
}

/* ptr-- then free */
void free_after_decrement(void) {
    char *buf = (char *)malloc(100);
    buf--;
    free(buf);
}

/* ptr += N then free */
void free_after_plus_equal(void) {
    char *buf = (char *)malloc(100);
    buf += 10;
    free(buf);
}

/* ptr -= N then free */
void free_after_minus_equal(void) {
    char *buf = (char *)malloc(100);
    buf -= 5;
    free(buf);
}

/* ++ptr then free */
void free_after_pre_increment(void) {
    char *buf = (char *)malloc(100);
    ++buf;
    free(buf);
}

/* --ptr then free */
void free_after_pre_decrement(void) {
    char *buf = (char *)malloc(100);
    --buf;
    free(buf);
}

/* calloc then modify then free */
void free_calloc_modified(void) {
    int *arr = (int *)calloc(10, sizeof(int));
    arr++;
    free(arr);
}

/* for loop modification then free */
void free_after_for_loop(void) {
    char *buf = (char *)malloc(100);
    for (int i = 0; i < 10; buf++) {
        i++;
    }
    free(buf);
}
