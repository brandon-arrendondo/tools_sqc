/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: unchecked_null_pointer.c
 *
 * This case demonstrates a violation where a function accepts a pointer
 * parameter without validating it for NULL before dereferencing.
 */

#include <stdio.h>
#include <string.h>

/* NON-COMPLIANT: Function doesn't validate pointer parameter */
int string_length(const char *str) {
    /* Direct use of str without NULL check */
    return strlen(str);  /* Potential NULL pointer dereference */
}

/* NON-COMPLIANT: Function doesn't validate multiple pointers */
void copy_data(char *dest, const char *src) {
    /* Using pointers without validation */
    strcpy(dest, src);  /* Both pointers could be NULL */
}

/* NON-COMPLIANT: Structure pointer not validated */
struct Person {
    char name[50];
    int age;
};

void print_person(struct Person *person) {
    /* Direct access without NULL check */
    printf("Name: %s, Age: %d\n", person->name, person->age);
}

/* NON-COMPLIANT: Array pointer not validated */
int sum_array(int *array, size_t size) {
    int sum = 0;
    /* Using array without NULL check */
    for (size_t i = 0; i < size; i++) {
        sum += array[i];  /* Potential NULL pointer access */
    }
    return sum;
}

/* NON-COMPLIANT: File pointer not validated */
void write_to_file(FILE *file, const char *data) {
    /* Writing without checking file validity */
    fprintf(file, "%s", data);  /* file could be NULL */
}

int main(void) {
    /* Examples of how these functions could fail */
    char *null_str = NULL;
    struct Person *null_person = NULL;
    int *null_array = NULL;
    FILE *null_file = NULL;

    /* These would all cause crashes or undefined behavior */
    // string_length(null_str);
    // copy_data(NULL, "test");
    // print_person(null_person);
    // sum_array(null_array, 10);
    // write_to_file(null_file, "test data");

    printf("Functions compiled but lack parameter validation\n");
    return 0;
}