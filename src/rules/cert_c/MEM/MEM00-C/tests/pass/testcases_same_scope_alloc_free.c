/*
 * Rule: MEM00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM00-C violation
 * Description: Allocation and free at same abstraction level
 */

#include <stdlib.h>
#include <string.h>

void process_data(void) {
    char *buffer = malloc(1024);
    if (buffer == NULL) return;

    strcpy(buffer, "processing");
    /* do work */

    free(buffer);
}

int compute(const int *data, int len) {
    int *tmp = calloc(len, sizeof(int));
    if (tmp == NULL) return -1;

    for (int i = 0; i < len; i++) {
        tmp[i] = data[i] * 2;
    }
    int result = tmp[0];

    free(tmp);
    return result;
}
