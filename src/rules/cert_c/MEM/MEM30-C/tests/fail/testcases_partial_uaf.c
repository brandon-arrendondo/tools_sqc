/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Frees part of allocated structure but accesses other parts
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char *data;
    int size;
} buffer_t;

int main() {
    buffer_t *buf = malloc(sizeof(buffer_t));
    if (buf == NULL) {
        return -1;
    }

    buf->data = malloc(100);
    buf->size = 100;

    strcpy(buf->data, "Test data");

    free(buf->data);
    // Don't free the struct itself

    // BUG: Access freed member
    printf("Data: %s\n", buf->data);
    printf("Size: %d\n", buf->size);

    free(buf);
    return 0;
}