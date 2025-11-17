/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char *buffer;
    size_t size;
    bool is_allocated;
} managed_buffer_t;

int process_data(size_t size, bool should_process) {
    managed_buffer_t buffer = {NULL, 0, false};

    // Conditional allocation
    if (should_process && size > 0) {
        buffer.buffer = malloc(size);
        if (buffer.buffer) {
            buffer.size = size;
            buffer.is_allocated = true;

            // Process data
            for (size_t i = 0; i < size; i++) {
                buffer.buffer[i] = 'A' + (i % 26);
            }

            printf("Processed %zu bytes\n", size);
        } else {
            printf("Allocation failed\n");
            return -1;
        }
    } else {
        printf("Processing skipped\n");
    }

    // Safe conditional cleanup - freed exactly once if allocated
    if (buffer.is_allocated && buffer.buffer) {
        free(buffer.buffer);
        buffer.buffer = NULL;
        buffer.is_allocated = false;
        printf("Buffer freed\n");
    }

    return 0;
}

int main() {
    // Test various scenarios
    process_data(100, true);   // Allocate and free
    process_data(200, false);  // No allocation, no free
    process_data(0, true);     // No allocation due to zero size
    process_data(300, true);   // Allocate and free

    return 0;
}