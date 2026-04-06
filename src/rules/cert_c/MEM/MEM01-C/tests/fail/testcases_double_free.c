/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM01-C violation
 * Description: Double-free patterns detected via CFG reachability
 */

#include <stdlib.h>

void simple_double_free(void) {
    char *p = malloc(100);
    free(p);  /* Violation: p is freed again below */
    free(p);
}

void double_free_with_intervening(void) {
    char *p = malloc(100);
    free(p);  /* Violation: p is freed again below */
    int x = 5;
    (void)x;
    free(p);
}

void conditional_double_free(int flag) {
    char *p = malloc(100);
    if (flag) {
        free(p);  /* Violation: p freed again in second if-block */
    }
    if (flag) {
        free(p);
    }
}
