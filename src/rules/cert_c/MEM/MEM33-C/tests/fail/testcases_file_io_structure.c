/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: file_io_structure.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to write
 * and read structures containing flexible array members using file I/O
 * operations without proper size handling.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 4;
    FILE *file;

    /* Proper allocation */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 7);
    }

    /* Write to file */
    file = fopen("test_flex.dat", "wb");
    if (file == NULL) {
        free(flex_struct);
        return 1;
    }

    /* VIOLATION: Writing only fixed members, not the flexible array */
    fwrite(flex_struct, sizeof(struct flex_array_struct), 1, file);
    fclose(file);

    /* Read from file */
    file = fopen("test_flex.dat", "rb");
    if (file == NULL) {
        free(flex_struct);
        return 1;
    }

    struct flex_array_struct *read_struct;
    read_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (read_struct == NULL) {
        fclose(file);
        free(flex_struct);
        return 1;
    }

    /* VIOLATION: Reading only fixed members */
    fread(read_struct, sizeof(struct flex_array_struct), 1, file);
    fclose(file);

    /* The flexible array data was not written/read properly */
    printf("Original data[0]: %d\n", flex_struct->data[0]);
    printf("Read data[0]: %d (garbage)\n", read_struct->data[0]);

    free(flex_struct);
    free(read_struct);
    return 0;
}