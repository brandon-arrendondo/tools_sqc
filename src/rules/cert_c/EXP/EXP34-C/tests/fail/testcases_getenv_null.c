/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using getenv result without checking for NULL
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *env_var = getenv("NONEXISTENT_VAR");

    // getenv returns NULL if environment variable doesn't exist
    printf("Length: %zu\n", strlen(env_var));
    printf("First char: %c\n", env_var[0]);

    return 0;
}