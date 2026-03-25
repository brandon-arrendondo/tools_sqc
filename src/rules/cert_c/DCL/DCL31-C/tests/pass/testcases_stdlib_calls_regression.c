/*
 * Rule: DCL31-C
 * Source: testcases
 * Status: PASS - Standard library calls should not be flagged as undeclared
 * Regression: Round 3 fix — std function database prevents false positives on known functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void use_standard_functions(void) {
    /* C11 standard library functions */
    printf("hello %s\n", "world");
    malloc(100);
    free(NULL);
    memcpy(NULL, NULL, 0);
    strlen("test");
    strcmp("a", "b");
    strncpy(NULL, NULL, 0);

    /* POSIX subset */
    fopen("test.txt", "r");
    fclose(NULL);
    snprintf(NULL, 0, "%d", 42);
    strtol("123", NULL, 10);
    atoi("42");
    abs(-1);
}
