/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM01-C violation
 * Description: Free followed by other statements before NULL assignment
 */

#include <stdlib.h>
#include <stdio.h>

void free_then_log(void) {
    char *buffer = malloc(128);
    if (buffer == NULL) return;

    free(buffer);  /* Violation: next stmt is not NULL assignment */
    printf("freed buffer\n");
    buffer = NULL;
}
