/*
 * Rule: FIO37-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO37-C violation
 * Description: strlen arithmetic on fgets result without empty check
 */

#include <stdio.h>
#include <string.h>

void strip_newline_unsafe(void) {
    char line[256];

    if (fgets(line, sizeof(line), stdin) != NULL) {
        line[strlen(line) - 1] = '\0';  /* Violation: strlen could be 0 */
    }
}
