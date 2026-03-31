/*
 * Rule: ERR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR00-C violation
 * Description: Allocation results properly checked
 */

#include <stdlib.h>
#include <string.h>

int safe_alloc(int count) {
    char *buf = malloc(256);
    if (buf == NULL) return -1;

    int *arr = calloc(count, sizeof(int));
    if (arr == NULL) {
        free(buf);
        return -1;
    }

    strcpy(buf, "safe");
    arr[0] = 1;

    free(arr);
    free(buf);
    return 0;
}
