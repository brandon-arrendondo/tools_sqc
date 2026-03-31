/*
 * Rule: STR06-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR06-C violation
 * Description: strtok on a copy of the string preserves original
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void safe_tokenize(void) {
    const char *original = getenv("PATH");
    if (original == NULL) return;

    char *copy = malloc(strlen(original) + 1);
    if (copy == NULL) return;
    strcpy(copy, original);

    char *tok = strtok(copy, ":");
    while (tok != NULL) {
        puts(tok);
        tok = strtok(NULL, ":");
    }

    free(copy);
}
