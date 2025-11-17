/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Only outer struct is freed, nested pointer memory is leaked
 */

#include <stdlib.h>
#include <string.h>

typedef struct {
    int size;
    char *data;
} Container;

void use_container() {
    Container *cont = malloc(sizeof(Container));
    if (cont == NULL) {
        return;
    }

    cont->size = 256;
    cont->data = malloc(cont->size);
    if (cont->data == NULL) {
        free(cont);
        return;
    }

    strcpy(cont->data, "Important data");

    // Only free the outer struct, not the nested data pointer
    free(cont);  // cont->data is leaked - MEMORY LEAK
}