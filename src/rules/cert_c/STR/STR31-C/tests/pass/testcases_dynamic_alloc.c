/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Dynamic allocation ensures sufficient space based on strlen calculation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char source[] = "Dynamic allocation example";
    char *dest;

    dest = malloc(strlen(source) + 1);  // Allocate exact space needed including null terminator
    if (dest != NULL) {
        strcpy(dest, source);
        printf("Copied: %s\n", dest);
        free(dest);
    }

    return 0;
}