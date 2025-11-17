/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Struct and its members are properly freed, no access after free
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char *name;
    int value;
} data_t;

void cleanup_data(data_t *data) {
    if (data != NULL) {
        free(data->name);
        data->name = NULL;
        free(data);
    }
}

int main() {
    data_t *data = malloc(sizeof(data_t));
    if (data == NULL) {
        return -1;
    }

    data->name = malloc(50);
    if (data->name == NULL) {
        free(data);
        return -1;
    }

    strcpy(data->name, "Test Data");
    data->value = 123;

    printf("Name: %s, Value: %d\n", data->name, data->value);

    cleanup_data(data);
    data = NULL;  // Prevent further access

    return 0;
}