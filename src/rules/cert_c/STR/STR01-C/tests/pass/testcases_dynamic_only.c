/*
 * Rule: STR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR01-C violation
 * Description: Consistent use of only dynamic allocation for strings
 */

#include <stdlib.h>
#include <string.h>

void dynamic_strings(const char *a, const char *b) {
    char *s1 = strdup(a);
    char *s2 = malloc(strlen(b) + 1);

    if (s1 && s2) {
        strcpy(s2, b);
    }

    free(s1);
    free(s2);
}
