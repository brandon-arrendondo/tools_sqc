/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Union containing pointer is freed but pointed-to memory is not
 */

#include <stdlib.h>

typedef union {
    int number;
    char *text;
} DataUnion;

void union_function() {
    DataUnion *data = malloc(sizeof(DataUnion));
    if (data == NULL) {
        return;
    }

    data->text = malloc(100);
    if (data->text == NULL) {
        free(data);
        return;
    }

    strcpy(data->text, "Union text data");

    // Free union but not the pointed-to memory
    free(data);  // data->text memory is leaked - MEMORY LEAK
}