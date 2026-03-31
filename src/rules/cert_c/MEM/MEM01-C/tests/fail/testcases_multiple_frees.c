/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM01-C violation
 * Description: Multiple pointers freed without setting to NULL
 */

#include <stdlib.h>
#include <string.h>

void multiple_pointers_not_nulled(void) {
    char *buf1 = malloc(100);
    char *buf2 = malloc(200);
    char *buf3 = malloc(50);

    if (buf1) strcpy(buf1, "hello");
    if (buf2) strcpy(buf2, "world");
    if (buf3) strcpy(buf3, "!");

    free(buf1);  /* Violation: not set to NULL */
    free(buf2);  /* Violation: not set to NULL */
    free(buf3);  /* Violation: not set to NULL */
}
