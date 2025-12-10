/*
 * Rule: MEM03-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM03-C violation
 * Description: realloc without clearing old data (shrinking can leak)
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void testcase_noncompliant_realloc_without_clear(void) {
    char *secret;

    /* Initialize secret */
    secret = (char *)malloc(100);
    if (!secret) return;

    size_t secret_size = strlen(secret);
    /* ... */
    if (secret_size > SIZE_MAX/2) {
        /* Handle error condition */
        free(secret);
        return;
    }
    secret = (char *)realloc(secret, secret_size * 2);  /* Violation: old data may not be cleared */
}
