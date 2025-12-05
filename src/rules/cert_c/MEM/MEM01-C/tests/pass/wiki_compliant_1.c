/*
 * Rule: MEM01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM01-C violation
 * Description: Pointer set to NULL after free prevents double-free
 */

#include <stdlib.h>

#define value_1 1
#define value_2 2

void compliant(void) {
    char *message = malloc(100);
    int message_type = value_1;

    if (message_type == value_1) {
        /* Process message type 1 */
        free(message);
        message = NULL;  /* Compliant: set to NULL after free */
    }
    /* ... */
    if (message_type == value_2) {
        /* Process message type 2 */
        free(message);
        message = NULL;  /* Compliant: set to NULL after free */
    }
}
