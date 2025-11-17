/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Nested structure freed in wrong order, inner memory leaked
 */

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int *values;
} DataStruct;

void wrong_free_order() {
    DataStruct *data = malloc(sizeof(DataStruct));
    if (data == NULL) {
        return;
    }

    data->name = malloc(50);
    data->values = malloc(10 * sizeof(int));

    if (data->name != NULL) {
        strcpy(data->name, "Test");
    }

    if (data->values != NULL) {
        data->values[0] = 42;
    }

    // Free in wrong order - should free inner pointers first
    free(data);        // Frees struct but loses pointers to inner memory
    // data->name and data->values are now leaked - MEMORY LEAK
}