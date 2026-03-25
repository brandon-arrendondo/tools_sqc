/*
 * Rule: API07-C
 * Source: testcases
 * Status: PASS - Proper free patterns (pointer not modified)
 */

#include <stdlib.h>
#include <string.h>

/* Free without modification */
void free_unmodified(void) {
    char *buf = (char *)malloc(100);
    memset(buf, 0, 100);
    free(buf);
}

/* Save original pointer before modifying */
void free_original_saved(void) {
    char *orig = (char *)malloc(100);
    char *cur = orig;
    cur++;
    free(orig);
}

/* Reassignment resets modification tracking */
void free_after_reassignment(void) {
    char *buf = (char *)malloc(100);
    buf++;
    buf = (char *)malloc(200);
    free(buf);
}

/* No allocation — no issue */
void no_alloc_free(char *input) {
    (void)input;
}

/* No void pointers, no strncpy */
void clean_function(void) {
    int x = 42;
    (void)x;
}
