/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: invalid_array_bounds.c
 *
 * This case demonstrates a violation where functions don't validate
 * array indices or size parameters, potentially leading to buffer overflows.
 */

#include <stdio.h>
#include <string.h>

/* NON-COMPLIANT: No validation of index parameter */
int get_element(int array[], size_t index) {
    /* Direct array access without bounds checking */
    return array[index];  /* index could be out of bounds */
}

/* NON-COMPLIANT: No validation of size parameter */
void clear_array(int *array, size_t size) {
    /* Using size without validation */
    for (size_t i = 0; i < size; i++) {
        array[i] = 0;  /* size could be larger than actual array */
    }
}

/* NON-COMPLIANT: No validation of offset and count */
void copy_range(char *dest, const char *src, size_t offset, size_t count) {
    /* Direct memory copy without bounds validation */
    memcpy(dest, src + offset, count);  /* offset+count could exceed src bounds */
}

/* NON-COMPLIANT: No validation of buffer capacity */
void append_string(char *buffer, const char *str, size_t buffer_size) {
    /* Appending without checking if there's enough space */
    strcat(buffer, str);  /* Could overflow if buffer_size is insufficient */
}

/* NON-COMPLIANT: No validation of position parameter */
void insert_at_position(int *array, size_t array_size, size_t position, int value) {
    /* Inserting without validating position */
    array[position] = value;  /* position could be >= array_size */
}

/* NON-COMPLIANT: No validation of slice parameters */
double calculate_average(double *data, size_t start, size_t end) {
    double sum = 0.0;
    /* Using range without validation */
    for (size_t i = start; i <= end; i++) {
        sum += data[i];  /* Could access beyond array bounds */
    }
    return sum / (end - start + 1);
}

int main(void) {
    int small_array[5] = {1, 2, 3, 4, 5};
    char buffer[10] = "Hello";
    double data[3] = {1.0, 2.0, 3.0};

    /* Examples of potentially dangerous calls */
    // get_element(small_array, 10);  /* Out of bounds */
    // clear_array(small_array, 1000);  /* Size too large */
    // copy_range(buffer, "source", 100, 50);  /* Invalid offset */
    // append_string(buffer, "Very long string that will overflow", 10);
    // insert_at_position(small_array, 5, 10, 999);  /* Position out of bounds */
    // calculate_average(data, 5, 10);  /* Invalid range */

    printf("Functions compiled but lack array bounds validation\n");
    return 0;
}