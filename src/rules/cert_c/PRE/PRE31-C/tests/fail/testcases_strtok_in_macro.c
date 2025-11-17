/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: strtok (side effect) in unsafe macro
 */

#include <string.h>

#define CHECK_NULL(ptr) ((ptr) != NULL)  /* UNSAFE */

void tokenize_string(char *str) {
    // strtok has side effect (modifies state) - may be called twice
    while (CHECK_NULL(strtok(str, " "))) {  // Line 13 - VIOLATION
        str = NULL;
    }
}

int main(void) {
    char buffer[] = "hello world test";
    tokenize_string(buffer);
    return 0;
}
