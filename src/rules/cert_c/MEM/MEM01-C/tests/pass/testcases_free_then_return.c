/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM01-C violation
 * Description: Free followed by return - pointer goes out of scope safely
 */

#include <stdlib.h>

void free_then_return(void) {
    char *p = malloc(100);
    if (p == NULL) return;
    /* Use p */
    free(p);
    return;
}

int free_then_return_value(void) {
    char *p = malloc(100);
    free(p);
    return 0;
}
