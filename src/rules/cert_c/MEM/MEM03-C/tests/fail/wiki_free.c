/*
 * Rule: MEM03-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM03-C violation
 * Description: Sensitive data freed without clearing first
 */

#include <stdlib.h>
#include <string.h>

void testcase_noncompliant_free_without_clear(void) {
    char *secret;
    /* Initialize secret to a null-terminated byte string,
       of less than SIZE_MAX chars */
    secret = (char *)malloc(100);
    if (!secret) return;

    size_t size = strlen(secret);
    char *new_secret;
    new_secret = (char *)malloc(size+1);
    if (!new_secret) {
        free(secret);
        return;
    }
    strcpy(new_secret, secret);

    /* Process new_secret... */

    free(new_secret);  /* Violation: sensitive data not cleared before free */
    new_secret = NULL;
    free(secret);
}
