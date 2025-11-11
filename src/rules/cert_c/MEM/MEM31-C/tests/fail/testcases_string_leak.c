/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Dynamically allocated string is never freed
 */

#include <stdlib.h>
#include <string.h>

char *build_message(const char *prefix, int number) {
    char *message = malloc(100);
    if (message == NULL) {
        return NULL;
    }

    sprintf(message, "%s: %d", prefix, number);
    return message;
}

void display_message() {
    char *msg = build_message("Count", 42);
    if (msg != NULL) {
        printf("%s\n", msg);
    }
    // String is never freed - MEMORY LEAK
}