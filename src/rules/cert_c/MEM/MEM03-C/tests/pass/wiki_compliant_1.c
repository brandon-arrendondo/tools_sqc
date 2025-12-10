/*
 * Rule: MEM03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM03-C violation
 * Description: memset_s used to clear sensitive data before free
 */

#include <stdlib.h>
#include <string.h>

void testcase_compliant_memset_before_free(void) {
    size_t size = 100;
    char *new_secret;
    /* Use calloc() to zero-out allocated space */
    new_secret = (char *)calloc(size+1, sizeof(char));
    if (!new_secret) {
        return;
    }

    /* Process new_secret... */

    /* Sanitize memory */
    memset_s(new_secret, '\0', size);  /* Clear before free */
    free(new_secret);
    new_secret = NULL;
}
