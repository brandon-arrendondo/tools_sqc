/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void safe_free(void **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} dynamic_array_t;

dynamic_array_t* create_array(size_t initial_capacity) {
    dynamic_array_t *arr = malloc(sizeof(dynamic_array_t));
    if (!arr) {
        return NULL;
    }

    arr->data = malloc(initial_capacity * sizeof(int));
    if (!arr->data) {
        free(arr);
        return NULL;
    }

    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

void destroy_array(dynamic_array_t **arr) {
    if (arr && *arr) {
        safe_free((void**)&((*arr)->data));
        safe_free((void**)arr);
    }
}

int main() {
    dynamic_array_t *array = create_array(10);

    if (array) {
        // Use the array
        for (size_t i = 0; i < 5; i++) {
            array->data[i] = i * 2;
            array->size++;
        }

        printf("Created array with %zu elements\n", array->size);

        // Safe cleanup - memory freed exactly once
        destroy_array(&array);

        // array is now NULL, safe to call destroy again
        destroy_array(&array);  // Safe - does nothing

        printf("Array destroyed safely\n");
    }

    return 0;
}