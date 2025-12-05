/*
 * Rule: MEM01-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM01-C violation
 * Description: Pointer not set to NULL after free can lead to double-free
 */

#include <stdlib.h>

#define value_1 1
#define value_2 2

void noncompliant(void) {
    char *message = malloc(100);
    int message_type = value_1;

    if (message_type == value_1) {
        /* Process message type 1 */
        free(message);  /* Violation: not set to NULL after free */
    }
    /* ...*/
    if (message_type == value_2) {
        /* Process message type 2 */
        free(message);  /* Potential double-free */
    }
}
