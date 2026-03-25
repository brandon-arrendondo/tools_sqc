/*
 * Rule: DCL07-C
 * Source: testcases
 * Status: PASS - Standard library calls should not be flagged for missing prototypes
 * Regression: Round 3 fix — std function database prevents false positives on known functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("hello\n");
    int *p = malloc(sizeof(int));
    free(p);
    char buf[64];
    memset(buf, 0, sizeof(buf));
    size_t len = strlen(buf);
    (void)len;
    return 0;
}
