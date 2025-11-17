/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Memory is freed at end of scope, preventing any further access
 */

#include <stdlib.h>
#include <stdio.h>

void process_data() {
    char *buffer = malloc(256);
    if (buffer == NULL) {
        return;
    }

    // Use the buffer
    snprintf(buffer, 256, "Processing data...");
    printf("%s\n", buffer);

    // Free at end of function scope
    free(buffer);
    // Function ends, no further access possible
}

int main() {
    process_data();
    return 0;
}