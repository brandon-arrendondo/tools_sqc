/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: strtok() operates on array, not string literal
 */

#include <string.h>

void tokenize(void) {
    // Compliant: array can be modified by strtok
    char str[] = "one,two,three";
    char *token = strtok(str, ",");
    while (token != NULL) {
        token = strtok(NULL, ",");
    }
}

int main(void) {
    tokenize();
    return 0;
}
