/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Token parsing into fixed buffer without length validation
 */

#include <stdio.h>
#include <string.h>

int main() {
    char input[] = "token1,very_long_token_that_exceeds_buffer,token3";
    char token_buffer[10];
    char *token;

    token = strtok(input, ",");
    while (token != NULL) {
        strcpy(token_buffer, token);  // Token might be longer than buffer
        printf("Token: %s\n", token_buffer);
        token = strtok(NULL, ",");
    }

    return 0;
}