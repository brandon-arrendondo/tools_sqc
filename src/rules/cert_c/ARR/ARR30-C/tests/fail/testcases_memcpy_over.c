/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: memcpy copies more data than destination buffer can hold
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    char src[20] = "This is a long string";
    char dest[10];

    // Copying 20 bytes to 10-byte buffer
    memcpy(dest, src, sizeof(src));

    dest[9] = '\0'; // Attempt to null-terminate
    printf("Destination: %s\n", dest);

    return 0;
}