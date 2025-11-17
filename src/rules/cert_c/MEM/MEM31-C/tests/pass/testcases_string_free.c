/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Dynamically allocated string is properly freed after use
 */

#include <stdlib.h>
#include <string.h>

char *create_greeting(const char *name) {
    size_t len = strlen("Hello, ") + strlen(name) + strlen("!") + 1;
    char *greeting = malloc(len);

    if (greeting == NULL) {
        return NULL;
    }

    strcpy(greeting, "Hello, ");
    strcat(greeting, name);
    strcat(greeting, "!");

    return greeting;
}

void greet_user() {
    char *message = create_greeting("Alice");
    if (message != NULL) {
        printf("%s\n", message);
        // Properly free the allocated string
        free(message);
    }
}