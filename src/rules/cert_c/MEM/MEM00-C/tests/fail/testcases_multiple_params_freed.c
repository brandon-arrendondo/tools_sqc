/*
 * Rule: MEM00-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM00-C violation
 * Description: Function frees multiple pointer parameters
 */

#include <stdlib.h>

void release_resources(char *buf, int *ids, double *vals) {
    free(buf);   /* Violation: freeing parameter */
    free(ids);   /* Violation: freeing parameter */
    free(vals);  /* Violation: freeing parameter */
}
