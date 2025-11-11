/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Nested structure with freed inner members accessed later
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char *name;
    struct {
        int *values;
        int count;
    } data;
} nested_t;

int main() {
    nested_t *obj = malloc(sizeof(nested_t));
    if (obj == NULL) {
        return -1;
    }

    obj->name = malloc(20);
    strcpy(obj->name, "Test");

    obj->data.values = malloc(3 * sizeof(int));
    obj->data.count = 3;

    for (int i = 0; i < 3; i++) {
        obj->data.values[i] = i + 10;
    }

    // Free inner structure
    free(obj->data.values);

    // BUG: Access freed nested member
    printf("First value: %d\n", obj->data.values[0]);

    free(obj->name);
    free(obj);
    return 0;
}