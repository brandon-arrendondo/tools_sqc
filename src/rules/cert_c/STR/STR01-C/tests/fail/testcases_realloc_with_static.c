/*
 * Rule: STR01-C
 * Source: testcases
 * Status: FAIL - Should trigger STR01-C violation
 * Description: Uses realloc alongside static char buffers
 */

#include <stdlib.h>
#include <string.h>

void build_message(int count) {
    char header[64] = "Results: ";    /* Static buffer */
    char *body = malloc(count * 20);  /* Dynamic allocation */

    if (body != NULL) {
        body[0] = '\0';
        for (int i = 0; i < count; i++) {
            char *tmp = realloc(body, (i + 1) * 20);
            if (tmp != NULL) body = tmp;
        }
        free(body);
    }
}
