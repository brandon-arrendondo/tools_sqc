/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Void pointer cast to array type allows out-of-bounds access
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    void *buffer = malloc(16);  // 16 bytes

    if (buffer != NULL) {
        // Cast to int array (4 ints = 16 bytes)
        int *int_array = (int *)buffer;

        // Access beyond allocated memory
        int_array[5] = 0x12345678;  // 5th int = 20 bytes > 16 bytes
        printf("int_array[6] = %d\n", int_array[6]);

        // Cast to char array
        char *char_array = (char *)buffer;
        char_array[20] = 'X';  // Beyond 16 bytes

        free(buffer);
    }

    return 0;
}