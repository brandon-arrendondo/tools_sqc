/*
 * Rule: EXP15-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP15-C violation
 *
 * while and for statements with proper bodies
 */

#include <stdio.h>

void proper_loop_bodies(int *data, int n) {
    /* COMPLIANT: while with proper body */
    int i = 0;
    while (i < n) {
        data[i] = 0;
        i++;
    }

    /* COMPLIANT: for with proper body */
    for (int j = 0; j < n; j++) {
        data[j] = j;
    }
}
