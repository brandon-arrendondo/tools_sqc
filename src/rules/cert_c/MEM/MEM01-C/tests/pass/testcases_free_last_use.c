/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM01-C violation
 * Description: Free as last use of pointer - no reuse, no danger
 */

#include <stdlib.h>

void free_at_end(void) {
    char *p = malloc(100);
    if (p == NULL) return;
    free(p);
}

void free_then_reassign(void) {
    char *p = malloc(100);
    free(p);
    p = malloc(200);
    free(p);
}

void free_different_pointers(void) {
    char *a = malloc(10);
    char *b = malloc(20);
    char *c = malloc(30);
    free(a);
    free(b);
    free(c);
}

void free_in_loop_scoped(int count) {
    for (int i = 0; i < count; i++) {
        char *tmp = malloc(64);
        if (tmp == NULL) continue;
        free(tmp);
    }
}
