/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: memory_operations_unsafe.c
 *
 * This case demonstrates violations where memory operation functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: No validation of size parameter */
void *allocate_memory(size_t size) {
    /* Direct allocation without checking size */
    return malloc(size);  /* size could be 0 or excessively large */
}

/* NON-COMPLIANT: No validation of pointer before reallocation */
void *resize_memory(void *ptr, size_t new_size) {
    /* Reallocating without validation */
    return realloc(ptr, new_size);  /* ptr validity not checked, new_size could be 0 */
}

/* NON-COMPLIANT: No validation of source and destination */
void copy_memory(void *dest, const void *src, size_t size) {
    /* Copying without validation */
    memcpy(dest, src, size);  /* dest or src could be NULL */
}

/* NON-COMPLIANT: No validation of memory region */
void clear_memory(void *ptr, size_t size) {
    /* Clearing without NULL check */
    memset(ptr, 0, size);  /* ptr could be NULL */
}

/* NON-COMPLIANT: No validation of overlapping regions */
void move_memory(void *dest, const void *src, size_t size) {
    /* Moving without checking for overlap properly */
    memcpy(dest, src, size);  /* Should use memmove for potentially overlapping regions */
}

/* NON-COMPLIANT: No validation of alignment requirements */
void *allocate_aligned(size_t alignment, size_t size) {
    /* Allocating without validating alignment */
    void *ptr;
    /* alignment might not be power of 2 or could be 0 */
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

/* NON-COMPLIANT: No validation of freed pointer */
void deallocate_memory(void **ptr) {
    /* Freeing without validation */
    free(*ptr);  /* *ptr might already be freed or invalid */
    *ptr = NULL;
}

/* NON-COMPLIANT: No validation of buffer capacity */
void fill_buffer(char *buffer, size_t buffer_size, char fill_char, size_t fill_count) {
    /* Filling without bounds checking */
    memset(buffer, fill_char, fill_count);  /* fill_count could exceed buffer_size */
}

int main(void) {
    void *null_ptr = NULL;
    char small_buffer[10];

    /* Examples of dangerous memory operations */
    // allocate_memory(0);  /* Zero size allocation */
    // allocate_memory(SIZE_MAX);  /* Excessive size */
    // resize_memory(null_ptr, 100);  /* NULL pointer reallocation */
    // copy_memory(null_ptr, small_buffer, 10);  /* NULL destination */
    // clear_memory(null_ptr, 100);  /* NULL pointer */
    // move_memory(small_buffer, small_buffer + 2, 8);  /* Overlapping regions */
    // allocate_aligned(3, 100);  /* Invalid alignment (not power of 2) */
    // void *freed_ptr = malloc(10);
    // free(freed_ptr);
    // deallocate_memory(&freed_ptr);  /* Double free */
    // fill_buffer(small_buffer, 10, 'A', 100);  /* Overflow */

    printf("Memory functions compiled but lack parameter validation\n");
    return 0;
}