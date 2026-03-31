/*
 * Rule: MEM00-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM00-C violation
 * Description: Helper function frees a pointer parameter
 */

#include <stdlib.h>
#include <string.h>

void cleanup_buffer(char *buf) {
    /* Violation: freeing parameter at wrong abstraction level */
    free(buf);
}

void process(void) {
    char *data = malloc(256);
    if (data == NULL) return;
    strcpy(data, "some data");
    cleanup_buffer(data);
}
