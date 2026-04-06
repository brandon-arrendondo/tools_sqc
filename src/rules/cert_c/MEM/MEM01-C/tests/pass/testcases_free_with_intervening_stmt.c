/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM01-C violation
 * Description: Free followed by log and NULL assignment - no unsafe use
 */

#include <stdlib.h>
#include <stdio.h>

void free_then_log(void) {
    char *buffer = malloc(128);
    if (buffer == NULL) return;

    free(buffer);  /* printf doesn't use buffer; then buffer = NULL */
    printf("freed buffer\n");
    buffer = NULL;
}
