/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Uses string functions on freed memory
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    char *str = malloc(50);
    if (str == NULL) {
        return -1;
    }

    strcpy(str, "Hello World");
    printf("String: %s\n", str);

    free(str);

    // BUG: Use freed string
    printf("Length: %zu\n", strlen(str));

    return 0;
}