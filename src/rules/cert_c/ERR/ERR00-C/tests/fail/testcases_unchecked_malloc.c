/*
 * Rule: ERR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR00-C violation
 * Description: Allocation return values not checked for errors
 */

#include <stdlib.h>
#include <string.h>

void unchecked_alloc(int count) {
    char *buf = malloc(256);     /* Violation: no NULL check */
    strcpy(buf, "data");

    int *arr = calloc(count, sizeof(int));  /* Violation: no NULL check */
    arr[0] = 42;

    free(arr);
    free(buf);
}
