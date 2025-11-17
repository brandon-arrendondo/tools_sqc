/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *buffer;
    size_t size;
} data_container_t;

void destroy_container(data_container_t *container) {
    if (container && container->buffer) {
        free(container->buffer);
        // Error: not setting to NULL allows double free
    }
}

int main() {
    data_container_t container;
    container.buffer = malloc(100 * sizeof(int));
    container.size = 100;

    if (container.buffer) {
        // Simulate exception/error condition
        printf("Processing data...\n");

        // Error path cleanup
        destroy_container(&container);

        // Normal path cleanup - double free
        destroy_container(&container);
    }

    return 0;
}