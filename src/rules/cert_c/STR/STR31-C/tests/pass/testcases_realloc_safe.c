/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Uses realloc to ensure sufficient space before string operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *buffer = malloc(10);
    char addition[] = " and more text";

    if (buffer) {
        strcpy(buffer, "Initial");

        // Reallocate to accommodate additional text
        size_t new_size = strlen(buffer) + strlen(addition) + 1;
        buffer = realloc(buffer, new_size);

        if (buffer) {
            strcat(buffer, addition);
            printf("Extended string: %s\n", buffer);
            free(buffer);
        }
    }

    return 0;
}