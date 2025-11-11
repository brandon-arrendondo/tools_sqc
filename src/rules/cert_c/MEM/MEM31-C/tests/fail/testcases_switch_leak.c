/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Switch statement missing break causes memory leak
 */

#include <stdlib.h>

void switch_function(int option) {
    char *buffer = malloc(1024);
    if (buffer == NULL) {
        return;
    }

    switch (option) {
        case 1:
            buffer[0] = 'A';
            free(buffer);
            break;
        case 2:
            buffer[0] = 'B';
            return;  // Early return without free - MEMORY LEAK
        case 3:
            buffer[0] = 'C';
            // Missing break and free - falls through
        default:
            printf("Default case\n");
            // buffer not freed in case 3 or default - MEMORY LEAK
    }
}