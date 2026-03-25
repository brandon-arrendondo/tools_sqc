/*
 * Rule: API07-C
 * Source: testcases
 * Status: PASS - Free at allocation start, no strncpy
 */

#include <stdlib.h>
#include <string.h>

/* Normal malloc/free without pointer modification */
void safe_malloc_free(void) {
    char *p = (char *)malloc(100);
    if (p == NULL) return;
    memset(p, 0, 100);
    free(p);
}

/* Reassignment resets modification tracking */
void reassignment_before_free(void) {
    char *p = (char *)malloc(100);
    if (p == NULL) return;
    p++;
    p = (char *)malloc(200);
    if (p == NULL) return;
    free(p);
}
