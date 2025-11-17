/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: String operations respect buffer size limits
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer[32];
    char input[100];

    printf("Enter text: ");
    fgets(input, sizeof(input), stdin);

    // Safe string copy with size limit
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    printf("Copied: %s\n", buffer);
    return 0;
}