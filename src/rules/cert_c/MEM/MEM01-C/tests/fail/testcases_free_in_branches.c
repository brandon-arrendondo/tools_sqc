/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM01-C violation
 * Description: Free in control flow branches without NULL assignment
 */

#include <stdlib.h>

void free_in_if_else(int condition) {
    char *data = malloc(256);
    if (data == NULL) return;

    if (condition) {
        /* Process one way */
        free(data);  /* Violation: not set to NULL */
    } else {
        /* Process another way */
        free(data);  /* Violation: not set to NULL */
    }
}

void free_in_loop(int count) {
    for (int i = 0; i < count; i++) {
        char *tmp = malloc(64);
        if (tmp == NULL) continue;
        /* Use tmp */
        free(tmp);  /* Violation: not set to NULL */
    }
}
