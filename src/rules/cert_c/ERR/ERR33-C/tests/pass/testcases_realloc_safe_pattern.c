/*
 * Rule: ERR33-C
 * Status: PASS - Safe realloc pattern: temp = realloc(ptr, size); if (temp) ptr = temp;
 */

#include <stdlib.h>

void f(void) {
    char *buf = malloc(100);
    if (buf == NULL) return;

    char *temp = realloc(buf, 200);
    if (temp == NULL) {
        free(buf);
        return;
    }
    buf = temp;
    free(buf);
}
