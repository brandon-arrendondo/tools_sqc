/*
 * Rule: MEM03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM03-C violation
 * Description: memset used to clear data before free
 */

#include <stdlib.h>
#include <string.h>

void testcase_compliant_clear_before_free(void) {
    char *secret;

    /* Initialize secret */
    secret = (char *)malloc(100);
    if (!secret) return;

    size_t secret_size = 50;

    /* Sanitize the buffer */
    memset((volatile char *)secret, '\0', secret_size);  /* Clear before free */

    free(secret);
    secret = NULL;
}
