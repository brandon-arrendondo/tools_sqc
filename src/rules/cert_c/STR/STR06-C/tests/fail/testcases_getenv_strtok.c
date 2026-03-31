/*
 * Rule: STR06-C
 * Source: testcases
 * Status: FAIL - Should trigger STR06-C violation
 * Description: strtok on getenv result corrupts environment
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void parse_path(void) {
    char *path = getenv("PATH");
    char *tok = strtok(path, ":");  /* Violation: modifies env string */
    while (tok != NULL) {
        puts(tok);
        tok = strtok(NULL, ":");
    }
}
