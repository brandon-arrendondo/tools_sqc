/*
 * Rule: STR04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR04-C violation
 * Description: Plain char used correctly for string operations
 */

#include <string.h>
#include <stdio.h>

void plain_char_operations(void) {
    char greeting[] = "Hello";
    char name[] = "World";
    char result[256];

    strcpy(result, greeting);
    strcat(result, " ");
    strcat(result, name);

    size_t len = strlen(result);
    printf("%s (len=%zu)\n", result, len);
}
