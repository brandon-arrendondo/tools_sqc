/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Function takes ownership of memory and frees it, caller doesn't access after
 */

#include <stdlib.h>
#include <stdio.h>

void consume_buffer(char *buffer) {
    if (buffer != NULL) {
        printf("Processing: %s\n", buffer);
        free(buffer);  // Function takes ownership and frees
    }
}

int main() {
    char *data = malloc(100);
    if (data == NULL) {
        return -1;
    }

    snprintf(data, 100, "Important data");

    // Pass ownership to function
    consume_buffer(data);
    data = NULL;  // Clear pointer to prevent accidental access

    // No access to freed memory
    return 0;
}