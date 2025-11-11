/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: strtok() modifies its argument, string literal passed
 */

#include <string.h>

void tokenize(void) {
    char *token = strtok("one,two,three", ",");  // Line 10 - VIOLATION: strtok modifies string literal
    while (token != NULL) {
        token = strtok(NULL, ",");
    }
}

int main(void) {
    tokenize();
    return 0;
}
