/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: All nested allocated memory (struct with pointer member) is freed
 */

#include <stdlib.h>
#include <string.h>

typedef struct {
    int size;
    char *data;
} DataContainer;

void use_container() {
    DataContainer *container = malloc(sizeof(DataContainer));
    if (container == NULL) {
        return;
    }

    container->size = 100;
    container->data = malloc(container->size);
    if (container->data == NULL) {
        free(container);  // Free outer struct if inner allocation fails
        return;
    }

    // Use the container
    strcpy(container->data, "Some important data");

    // Properly free both nested and outer memory
    free(container->data);
    free(container);
}