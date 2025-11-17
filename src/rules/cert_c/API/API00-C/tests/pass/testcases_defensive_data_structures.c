/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: defensive_data_structures.c
 *
 * This case demonstrates compliant code for data structure operations
 * with comprehensive parameter validation and error handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

/* Safe dynamic array structure */
typedef struct {
    void **elements;
    size_t size;
    size_t capacity;
    size_t element_size;
} SafeArray;

/* Safe linked list node */
typedef struct SafeNode {
    void *data;
    size_t data_size;
    struct SafeNode *next;
} SafeNode;

/* Safe linked list structure */
typedef struct {
    SafeNode *head;
    SafeNode *tail;
    size_t count;
    size_t max_size;  /* Optional size limit */
} SafeList;

/* COMPLIANT: Safe array creation with validation */
SafeArray *safe_array_create(size_t initial_capacity, size_t element_size) {
    /* Validate parameters */
    if (initial_capacity == 0 || element_size == 0) {
        errno = EINVAL;
        return NULL;
    }

    /* Check for potential overflow */
    if (initial_capacity > SIZE_MAX / sizeof(void *) ||
        initial_capacity > SIZE_MAX / element_size) {
        errno = ERANGE;
        return NULL;
    }

    /* Allocate array structure */
    SafeArray *array = malloc(sizeof(SafeArray));
    if (!array) {
        errno = ENOMEM;
        return NULL;
    }

    /* Allocate elements array */
    array->elements = calloc(initial_capacity, sizeof(void *));
    if (!array->elements) {
        free(array);
        errno = ENOMEM;
        return NULL;
    }

    array->size = 0;
    array->capacity = initial_capacity;
    array->element_size = element_size;

    return array;
}

/* COMPLIANT: Safe array element access with bounds checking */
int safe_array_get(const SafeArray *array, size_t index, void *element) {
    /* Validate parameters */
    if (!array || !element) {
        errno = EINVAL;
        return -1;
    }

    /* Check bounds */
    if (index >= array->size) {
        errno = ERANGE;
        return -1;
    }

    /* Validate element exists */
    if (!array->elements[index]) {
        errno = ENOENT;
        return -1;
    }

    /* Copy element data */
    memcpy(element, array->elements[index], array->element_size);
    return 0;
}

/* COMPLIANT: Safe array element setting with validation */
int safe_array_set(SafeArray *array, size_t index, const void *element) {
    /* Validate parameters */
    if (!array || !element) {
        errno = EINVAL;
        return -1;
    }

    /* Check bounds */
    if (index >= array->size) {
        errno = ERANGE;
        return -1;
    }

    /* Allocate memory for element if needed */
    if (!array->elements[index]) {
        array->elements[index] = malloc(array->element_size);
        if (!array->elements[index]) {
            errno = ENOMEM;
            return -1;
        }
    }

    /* Copy element data */
    memcpy(array->elements[index], element, array->element_size);
    return 0;
}

/* COMPLIANT: Safe array resize with overflow checking */
int safe_array_resize(SafeArray *array, size_t new_capacity) {
    /* Validate parameters */
    if (!array) {
        errno = EINVAL;
        return -1;
    }

    if (new_capacity == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Check for overflow */
    if (new_capacity > SIZE_MAX / sizeof(void *)) {
        errno = ERANGE;
        return -1;
    }

    /* Don't shrink below current size */
    if (new_capacity < array->size) {
        errno = EINVAL;
        return -1;
    }

    /* Attempt reallocation */
    void **new_elements = realloc(array->elements, new_capacity * sizeof(void *));
    if (!new_elements) {
        errno = ENOMEM;
        return -1;  /* Original array unchanged */
    }

    /* Initialize new elements to NULL */
    for (size_t i = array->capacity; i < new_capacity; i++) {
        new_elements[i] = NULL;
    }

    array->elements = new_elements;
    array->capacity = new_capacity;
    return 0;
}

/* COMPLIANT: Safe list creation with optional size limit */
SafeList *safe_list_create(size_t max_size) {
    /* max_size of 0 means unlimited */
    SafeList *list = malloc(sizeof(SafeList));
    if (!list) {
        errno = ENOMEM;
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    list->max_size = max_size;  /* 0 = unlimited */

    return list;
}

/* COMPLIANT: Safe list append with validation */
int safe_list_append(SafeList *list, const void *data, size_t data_size) {
    /* Validate parameters */
    if (!list || !data) {
        errno = EINVAL;
        return -1;
    }

    if (data_size == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Check size limit */
    if (list->max_size > 0 && list->count >= list->max_size) {
        errno = ENOSPC;
        return -1;
    }

    /* Check for count overflow */
    if (list->count == SIZE_MAX) {
        errno = ERANGE;
        return -1;
    }

    /* Allocate new node */
    SafeNode *new_node = malloc(sizeof(SafeNode));
    if (!new_node) {
        errno = ENOMEM;
        return -1;
    }

    /* Allocate data storage */
    new_node->data = malloc(data_size);
    if (!new_node->data) {
        free(new_node);
        errno = ENOMEM;
        return -1;
    }

    /* Copy data */
    memcpy(new_node->data, data, data_size);
    new_node->data_size = data_size;
    new_node->next = NULL;

    /* Add to list */
    if (list->tail) {
        list->tail->next = new_node;
        list->tail = new_node;
    } else {
        list->head = list->tail = new_node;
    }

    list->count++;
    return 0;
}

/* COMPLIANT: Safe list search with validation */
SafeNode *safe_list_find(const SafeList *list, const void *data, size_t data_size,
                        int (*compare)(const void *a, const void *b, size_t size)) {
    /* Validate parameters */
    if (!list || !data || !compare) {
        errno = EINVAL;
        return NULL;
    }

    if (data_size == 0) {
        errno = EINVAL;
        return NULL;
    }

    /* Search through list */
    SafeNode *current = list->head;
    while (current) {
        /* Only compare if sizes match */
        if (current->data_size == data_size &&
            compare(current->data, data, data_size) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;  /* Not found */
}

/* COMPLIANT: Safe list removal with validation */
int safe_list_remove(SafeList *list, const void *data, size_t data_size,
                    int (*compare)(const void *a, const void *b, size_t size)) {
    /* Validate parameters */
    if (!list || !data || !compare) {
        errno = EINVAL;
        return -1;
    }

    if (data_size == 0) {
        errno = EINVAL;
        return -1;
    }

    SafeNode *current = list->head;
    SafeNode *previous = NULL;

    while (current) {
        if (current->data_size == data_size &&
            compare(current->data, data, data_size) == 0) {

            /* Found node to remove */
            if (previous) {
                previous->next = current->next;
            } else {
                list->head = current->next;
            }

            if (current == list->tail) {
                list->tail = previous;
            }

            /* Free node resources */
            free(current->data);
            free(current);
            list->count--;
            return 0;
        }

        previous = current;
        current = current->next;
    }

    errno = ENOENT;
    return -1;  /* Not found */
}

/* COMPLIANT: Safe memory cleanup functions */
void safe_array_destroy(SafeArray *array) {
    if (!array) {
        return;  /* Safe to call on NULL */
    }

    /* Free all elements */
    if (array->elements) {
        for (size_t i = 0; i < array->size; i++) {
            free(array->elements[i]);
        }
        free(array->elements);
    }

    free(array);
}

void safe_list_destroy(SafeList *list) {
    if (!list) {
        return;  /* Safe to call on NULL */
    }

    /* Free all nodes */
    SafeNode *current = list->head;
    while (current) {
        SafeNode *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }

    free(list);
}

/* Helper function for demonstrations */
int int_compare(const void *a, const void *b, size_t size) {
    if (size != sizeof(int)) {
        return -1;  /* Size mismatch */
    }
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return (*ia == *ib) ? 0 : (*ia < *ib ? -1 : 1);
}

int main(void) {
    printf("=== Safe Data Structure Operations ===\n\n");

    /* Demonstrate safe array operations */
    SafeArray *array = safe_array_create(10, sizeof(int));
    if (!array) {
        printf("Failed to create array: %s\n", strerror(errno));
        return 1;
    }

    /* Add some elements */
    for (int i = 0; i < 5; i++) {
        if (safe_array_set(array, i, &i) != 0) {
            printf("Failed to set array element %d: %s\n", i, strerror(errno));
        }
    }
    array->size = 5;  /* Update size after adding elements */

    /* Access elements */
    for (size_t i = 0; i < array->size; i++) {
        int value;
        if (safe_array_get(array, i, &value) == 0) {
            printf("Array[%zu] = %d\n", i, value);
        }
    }

    /* Test bounds checking */
    int value;
    if (safe_array_get(array, 100, &value) != 0) {
        printf("Correctly rejected out-of-bounds access: %s\n", strerror(errno));
    }

    /* Demonstrate safe list operations */
    SafeList *list = safe_list_create(100);  /* Max 100 elements */
    if (!list) {
        printf("Failed to create list: %s\n", strerror(errno));
        safe_array_destroy(array);
        return 1;
    }

    /* Add some elements to list */
    for (int i = 10; i < 15; i++) {
        if (safe_list_append(list, &i, sizeof(int)) != 0) {
            printf("Failed to append to list: %s\n", strerror(errno));
        }
    }

    printf("List count: %zu\n", list->count);

    /* Search for an element */
    int search_value = 12;
    SafeNode *found = safe_list_find(list, &search_value, sizeof(int), int_compare);
    if (found) {
        printf("Found value %d in list\n", *(int *)found->data);
    } else {
        printf("Value not found in list\n");
    }

    /* Remove an element */
    if (safe_list_remove(list, &search_value, sizeof(int), int_compare) == 0) {
        printf("Successfully removed value %d from list\n", search_value);
        printf("New list count: %zu\n", list->count);
    }

    /* Clean up */
    safe_array_destroy(array);
    safe_list_destroy(list);

    printf("\n=== Data structure operations completed ===\n");
    return 0;
}