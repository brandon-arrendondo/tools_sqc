/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: file_io_compliant.c
 *
 * This case demonstrates compliant code that properly handles file I/O
 * operations with structures containing flexible array members by
 * writing and reading both fixed and flexible parts correctly.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *original, *loaded;
    size_t array_size = 4;
    FILE *file;
    const char *filename = "flex_data.bin";

    /* COMPLIANT: Proper dynamic allocation */
    original = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (original == NULL) return 1;

    /* Initialize the structure */
    original->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        original->data[i] = (int)(i * 7 + 3);
    }

    printf("Original structure:\n");
    printf("num: %zu\n", original->num);
    printf("data: ");
    for (size_t i = 0; i < original->num; i++) {
        printf("%d ", original->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Write both fixed and flexible parts to file */
    file = fopen(filename, "wb");
    if (file == NULL) {
        free(original);
        return 1;
    }

    /* Write the fixed part */
    fwrite(&original->num, sizeof(size_t), 1, file);

    /* Write the flexible array part */
    fwrite(original->data, sizeof(int), original->num, file);

    fclose(file);

    /* COMPLIANT: Read back the structure */
    file = fopen(filename, "rb");
    if (file == NULL) {
        free(original);
        return 1;
    }

    /* Read the size first */
    size_t loaded_size;
    if (fread(&loaded_size, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        free(original);
        return 1;
    }

    /* COMPLIANT: Allocate space based on the size read from file */
    loaded = malloc(sizeof(struct flex_array_struct) + sizeof(int) * loaded_size);
    if (loaded == NULL) {
        fclose(file);
        free(original);
        return 1;
    }

    loaded->num = loaded_size;

    /* Read the flexible array data */
    if (fread(loaded->data, sizeof(int), loaded->num, file) != loaded->num) {
        fclose(file);
        free(original);
        free(loaded);
        return 1;
    }

    fclose(file);

    /* Verify the loaded data */
    printf("\nLoaded structure:\n");
    printf("num: %zu\n", loaded->num);
    printf("data: ");
    for (size_t i = 0; i < loaded->num; i++) {
        printf("%d ", loaded->data[i]);
    }
    printf("\n");

    /* Compare original and loaded */
    int match = (original->num == loaded->num);
    for (size_t i = 0; i < original->num && match; i++) {
        if (original->data[i] != loaded->data[i]) {
            match = 0;
        }
    }

    printf("Data %s\n", match ? "matches" : "differs");

    /* COMPLIANT: Proper cleanup */
    free(original);
    free(loaded);
    remove(filename);

    return 0;
}