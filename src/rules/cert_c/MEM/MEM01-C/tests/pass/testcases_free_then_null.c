/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM01-C violation
 * Description: Proper pattern: free immediately followed by NULL assignment
 */

#include <stdlib.h>
#include <string.h>

void proper_cleanup(void) {
    char *buf1 = malloc(100);
    char *buf2 = malloc(200);

    if (buf1) strcpy(buf1, "hello");
    if (buf2) strcpy(buf2, "world");

    free(buf1);
    buf1 = NULL;

    free(buf2);
    buf2 = NULL;
}

void proper_cleanup_with_zero(void) {
    char *data = malloc(64);
    if (data == NULL) return;

    free(data);
    data = 0;  /* Also acceptable: assign 0 instead of NULL */
}
