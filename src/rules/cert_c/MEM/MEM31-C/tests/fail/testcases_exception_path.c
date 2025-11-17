/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Exception/error path doesn't free allocated memory
 */

#include <stdlib.h>
#include <stdio.h>

int read_file(const char *filename) {
    char *buffer = malloc(1024);
    if (buffer == NULL) {
        return -1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -2;  // Error return without freeing buffer - MEMORY LEAK
    }

    size_t bytes_read = fread(buffer, 1, 1024, file);
    fclose(file);

    printf("Read %zu bytes\n", bytes_read);
    free(buffer);  // Only freed on success path
    return 0;
}