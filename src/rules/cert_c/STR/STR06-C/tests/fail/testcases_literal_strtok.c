/*
 * Rule: STR06-C
 * Source: testcases
 * Status: FAIL - Should trigger STR06-C violation
 * Description: String literal passed to strtok is undefined behavior
 */

#include <string.h>
#include <stdio.h>

void tokenize_literal(void) {
    char *tok = strtok("one,two,three", ",");  /* Violation: literal is immutable */
    while (tok != NULL) {
        puts(tok);
        tok = strtok(NULL, ",");
    }
}
