/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM01-C violation
 * Description: Free in branches with proper NULL assignment
 */

#include <stdlib.h>

void cleanup_in_branches(int mode) {
    char *data = malloc(256);
    if (data == NULL) return;

    if (mode == 1) {
        free(data);
        data = NULL;
    } else if (mode == 2) {
        free(data);
        data = NULL;
    } else {
        free(data);
        data = NULL;
    }
}
