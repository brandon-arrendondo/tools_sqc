/*
 * Rule: FIO37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO37-C violation
 * Description: Safe newline removal using strchr
 */

#include <stdio.h>
#include <string.h>

void strip_newline_safe(void) {
    char buf[256];

    if (fgets(buf, sizeof(buf), stdin) != NULL) {
        char *nl = strchr(buf, '\n');
        if (nl != NULL) {
            *nl = '\0';
        }
    }
}
