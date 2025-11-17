/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: file_operations_unchecked.c
 *
 * This case demonstrates violations where file operation functions
 * don't validate their file-related parameters.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: No validation of file pointer */
long get_file_size(FILE *file) {
    /* Using file pointer without validation */
    fseek(file, 0, SEEK_END);  /* file could be NULL */
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

/* NON-COMPLIANT: No validation of filename */
FILE *open_file(const char *filename, const char *mode) {
    /* Opening file without validating filename */
    return fopen(filename, mode);  /* filename could be NULL or empty */
}

/* NON-COMPLIANT: No validation of file state */
int read_next_byte(FILE *file) {
    /* Reading without checking file state */
    return fgetc(file);  /* file could be NULL or in error state */
}

/* NON-COMPLIANT: No validation of buffer or file */
size_t read_file_data(FILE *file, char *buffer, size_t size) {
    /* Reading without parameter validation */
    return fread(buffer, 1, size, file);  /* buffer or file could be NULL */
}

/* NON-COMPLIANT: No validation of position parameter */
void seek_to_position(FILE *file, long position) {
    /* Seeking without validation */
    fseek(file, position, SEEK_SET);  /* position could be negative or beyond file */
}

/* NON-COMPLIANT: No validation of line buffer */
char *read_line(FILE *file, char *buffer, int size) {
    /* Reading line without validation */
    return fgets(buffer, size, file);  /* buffer could be NULL, size could be <= 0 */
}

/* NON-COMPLIANT: No validation of format or file */
void write_formatted(FILE *file, const char *format, const char *data) {
    /* Writing without validation */
    fprintf(file, format, data);  /* file or format could be NULL */
}

/* NON-COMPLIANT: No validation of file permissions */
void write_binary_data(const char *filename, void *data, size_t size) {
    /* Opening and writing without checks */
    FILE *file = fopen(filename, "wb");  /* No check if file opened successfully */
    fwrite(data, 1, size, file);  /* file could be NULL */
    fclose(file);  /* Closing NULL file */
}

int main(void) {
    FILE *null_file = NULL;
    char *null_buffer = NULL;

    /* Examples of dangerous file operations */
    // get_file_size(null_file);  /* NULL file pointer */
    // open_file(NULL, "r");  /* NULL filename */
    // read_next_byte(null_file);  /* NULL file pointer */
    // read_file_data(null_file, null_buffer, 100);  /* NULL parameters */
    // seek_to_position(null_file, -1000);  /* Invalid position */
    // read_line(null_file, null_buffer, -10);  /* Invalid parameters */
    // write_formatted(null_file, NULL, "data");  /* NULL parameters */
    // write_binary_data("/invalid/path/file.bin", NULL, 100);  /* Invalid path and NULL data */

    printf("File functions compiled but lack parameter validation\n");
    return 0;
}