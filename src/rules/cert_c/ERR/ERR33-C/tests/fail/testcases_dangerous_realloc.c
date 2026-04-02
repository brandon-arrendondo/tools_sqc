/*
 * Rule: ERR33-C
 * Status: FAIL - Dangerous realloc: ptr = realloc(ptr, size) without NULL check
 */

#include <stdlib.h>

void f(void) {
    char *buf = malloc(100);
    buf = realloc(buf, 200);  /* VIOLATION: dangerous realloc overwrites original pointer */
}
