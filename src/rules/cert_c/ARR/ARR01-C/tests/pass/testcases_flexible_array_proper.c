/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>
#include <stdlib.h>

struct dynamic_array {
    size_t count;
    int data[];
};

struct dynamic_array* create_array(size_t size) {
    struct dynamic_array *arr = malloc(sizeof(struct dynamic_array) + size * sizeof(int));
    if (arr) {
        arr->count = size;
        for (size_t i = 0; i < size; i++) {
            arr->data[i] = i + 1;
        }
    }
    return arr;
}

void process_dynamic_array(struct dynamic_array *arr) {
    if (!arr) return;

    for (size_t i = 0; i < arr->count; i++) {
        arr->data[i] *= 2;
    }
}

int main() {
    struct dynamic_array *arr = create_array(10);
    if (arr) {
        process_dynamic_array(arr);

        printf("Dynamic array contents: ");
        for (size_t i = 0; i < arr->count; i++) {
            printf("%d ", arr->data[i]);
        }
        printf("\n");

        free(arr);
    }

    return 0;
}