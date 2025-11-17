/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Memory is freed in both success and error paths
 */

#include <stdlib.h>
#include <stdio.h>

int process_file(const char *filename) {
    char *buffer = malloc(1024);
    if (buffer == NULL) {
        return -1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        free(buffer);  // Free on error path
        return -2;
    }

    // Read and process file
    size_t bytes_read = fread(buffer, 1, 1024, file);
    fclose(file);

    if (bytes_read == 0) {
        free(buffer);  // Free on another error path
        return -3;
    }

    // Process the data
    printf("Processed %zu bytes\n", bytes_read);

    free(buffer);  // Free on success path
    return 0;
}