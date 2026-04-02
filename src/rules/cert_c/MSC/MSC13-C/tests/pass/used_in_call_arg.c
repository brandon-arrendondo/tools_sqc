/*
 * Rule: MSC13-C
 * Status: PASS - Variable used as function call argument
 */

#include <stdlib.h>
#include <string.h>

void f(void) {
    char buf[64];
    int len = 32;
    memset(buf, 0, len);
}
