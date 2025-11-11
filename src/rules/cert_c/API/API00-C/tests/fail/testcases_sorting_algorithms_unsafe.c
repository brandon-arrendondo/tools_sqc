/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: sorting_algorithms_unsafe.c
 *
 * This case demonstrates violations where sorting and searching functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: No validation of array or comparison function */
void bubble_sort(int *array, size_t size, int (*compare)(const void *, const void *)) {
    /* No validation of array or compare function */
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {
            if (compare(&array[j], &array[j + 1]) > 0) {  /* array or compare could be NULL */
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

/* NON-COMPLIANT: No validation of binary search parameters */
int binary_search(int *array, size_t size, int target) {
    /* No validation of array */
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (array[mid] == target) {  /* array could be NULL */
            return mid;
        }
        if (array[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/* NON-COMPLIANT: No validation of quicksort parameters */
void quicksort(int *array, int low, int high) {
    /* No validation of array or bounds */
    if (low < high) {
        int pivot = partition(array, low, high);  /* array could be NULL */
        quicksort(array, low, pivot - 1);
        quicksort(array, pivot + 1, high);
    }
}

/* Helper function for quicksort - also unsafe */
int partition(int *array, int low, int high) {
    /* No validation of array */
    int pivot = array[high];  /* array could be NULL */
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (array[j] < pivot) {
            i++;
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
    int temp = array[i + 1];
    array[i + 1] = array[high];
    array[high] = temp;
    return i + 1;
}

/* NON-COMPLIANT: No validation of merge sort parameters */
void merge_sort(int *array, int left, int right) {
    /* No validation of array */
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(array, left, mid);  /* array could be NULL */
        merge_sort(array, mid + 1, right);
        merge(array, left, mid, right);
    }
}

/* Helper function for merge sort - also unsafe */
void merge(int *array, int left, int mid, int right) {
    /* No validation of array or bounds */
    int left_size = mid - left + 1;
    int right_size = right - mid;

    int *left_array = malloc(left_size * sizeof(int));
    int *right_array = malloc(right_size * sizeof(int));

    /* Copying without validation */
    for (int i = 0; i < left_size; i++) {
        left_array[i] = array[left + i];  /* array could be NULL */
    }
    for (int j = 0; j < right_size; j++) {
        right_array[j] = array[mid + 1 + j];
    }

    /* Merge logic without validation */
    int i = 0, j = 0, k = left;
    while (i < left_size && j < right_size) {
        if (left_array[i] <= right_array[j]) {
            array[k] = left_array[i];
            i++;
        } else {
            array[k] = right_array[j];
            j++;
        }
        k++;
    }

    free(left_array);
    free(right_array);
}

/* NON-COMPLIANT: No validation of search key or array */
void *linear_search(void *array, size_t count, size_t element_size,
                   const void *key, int (*compare)(const void *, const void *)) {
    /* No validation of any parameters */
    char *current = (char *)array;
    for (size_t i = 0; i < count; i++) {
        if (compare(current, key) == 0) {  /* array, key, or compare could be NULL */
            return current;
        }
        current += element_size;
    }
    return NULL;
}

int main(void) {
    int *null_array = NULL;
    int (*null_compare)(const void *, const void *) = NULL;

    /* Examples of dangerous sorting operations */
    // bubble_sort(null_array, 10, null_compare);  /* NULL array and compare */
    // binary_search(null_array, 10, 5);  /* NULL array */
    // quicksort(null_array, 0, 9);  /* NULL array */
    // merge_sort(null_array, 0, 9);  /* NULL array */
    // linear_search(null_array, 10, sizeof(int), &(int){5}, null_compare);  /* NULL parameters */

    printf("Sorting functions compiled but lack parameter validation\n");
    return 0;
}