/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>
#include <stdlib.h>

struct container {
    int count;
    int data[];
};

void wrong_size_calculation(struct container *c) {
    size_t data_size = sizeof(c->data);

    printf("Wrong data size: %zu\n", data_size);
}

int main() {
    struct container *c = malloc(sizeof(struct container) + 10 * sizeof(int));
    if (c) {
        c->count = 10;
        wrong_size_calculation(c);
        free(c);
    }

    return 0;
}