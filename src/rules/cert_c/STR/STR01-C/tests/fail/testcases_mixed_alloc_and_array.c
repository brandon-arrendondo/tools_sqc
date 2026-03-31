/*
 * Rule: STR01-C
 * Source: testcases
 * Status: FAIL - Should trigger STR01-C violation
 * Description: Function uses both static char arrays and dynamic allocation
 */

#include <stdlib.h>
#include <string.h>

void process_names(const char *input) {
    char fixed_buf[128] = "prefix: ";   /* Static array */
    char *dynamic = strdup(input);       /* Dynamic allocation */

    if (dynamic != NULL) {
        strncat(fixed_buf, dynamic, sizeof(fixed_buf) - strlen(fixed_buf) - 1);
        free(dynamic);
    }
}
