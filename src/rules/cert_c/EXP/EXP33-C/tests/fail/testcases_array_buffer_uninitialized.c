/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: array_buffer_uninitialized.c
 *
 * This case demonstrates violations involving uninitialized arrays
 * and buffers, including partial initialization issues.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Uninitialized array access */
void process_array_data(void) {
    int numbers[10];  /* Uninitialized array */

    /* Reading uninitialized array elements */
    for (int i = 0; i < 10; i++) {
        printf("Number[%d]: %d\n", i, numbers[i]);  /* Undefined behavior */
    }

    /* Only initializing some elements */
    numbers[0] = 1;
    numbers[1] = 2;
    /* Elements 2-9 remain uninitialized */

    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += numbers[i];  /* Reading uninitialized elements */
    }
    printf("Sum: %d\n", sum);
}

/* NON-COMPLIANT: Partial buffer initialization */
void unsafe_buffer_operations(void) {
    char buffer[256];  /* Uninitialized buffer */

    /* Only partial initialization */
    buffer[0] = 'H';
    buffer[1] = 'e';
    buffer[2] = 'l';
    /* Rest of buffer uninitialized */

    /* Reading entire buffer as string */
    printf("Buffer content: %s\n", buffer);  /* Undefined behavior - no null terminator */

    /* Using uninitialized buffer in operations */
    int len = strlen(buffer);  /* Undefined behavior */
    printf("Length: %d\n", len);
}

/* NON-COMPLIANT: Multidimensional array issues */
void matrix_operations(void) {
    int matrix[3][3];  /* Uninitialized 2D array */

    /* Partial initialization of only first row */
    for (int j = 0; j < 3; j++) {
        matrix[0][j] = j + 1;
    }
    /* Rows 1 and 2 remain uninitialized */

    /* Reading entire matrix */
    printf("Matrix:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);  /* Reading uninitialized elements */
        }
        printf("\n");
    }
}

/* NON-COMPLIANT: Variable-length array issues */
void vla_problems(int size) {
    int vla[size];  /* Uninitialized variable-length array */

    if (size > 0) {
        /* Only initialize first element */
        vla[0] = 42;

        /* Reading all elements */
        for (int i = 0; i < size; i++) {
            printf("VLA[%d]: %d\n", i, vla[i]);  /* Reading uninitialized elements */
        }
    }
}

/* NON-COMPLIANT: Array passed to function without initialization */
void process_grades(int grades[], int count) {
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += grades[i];  /* May read uninitialized data */
    }

    double average = (double)total / count;
    printf("Average grade: %.2f\n", average);
}

void test_uninitialized_grades(void) {
    int student_grades[5];  /* Uninitialized array */

    /* Only set some grades */
    student_grades[0] = 85;
    student_grades[2] = 92;
    /* grades[1], grades[3], grades[4] uninitialized */

    process_grades(student_grades, 5);  /* Undefined behavior */
}

/* NON-COMPLIANT: Character array string operations */
void string_manipulation_errors(void) {
    char name[50];  /* Uninitialized character array */

    /* Trying to append to uninitialized string */
    strcat(name, " Smith");  /* Undefined behavior - name not initialized */

    printf("Name: %s\n", name);

    /* Another uninitialized string issue */
    char filename[100];  /* Uninitialized */

    /* Conditional initialization */
    if (rand() % 2) {
        strcpy(filename, "data.txt");
    }
    /* filename may be uninitialized */

    FILE *file = fopen(filename, "r");  /* May use uninitialized filename */
    if (file) {
        fclose(file);
    }
}

int main(void) {
    printf("=== Array and Buffer Uninitialized Demo ===\n");

    /* Test 1: Uninitialized array processing */
    printf("1. Array data processing:\n");
    process_array_data();

    /* Test 2: Partial buffer initialization */
    printf("\n2. Buffer operations:\n");
    unsafe_buffer_operations();

    /* Test 3: Matrix operations */
    printf("\n3. Matrix operations:\n");
    matrix_operations();

    /* Test 4: Variable-length array */
    printf("\n4. VLA problems:\n");
    vla_problems(5);

    /* Test 5: Uninitialized grades */
    printf("\n5. Uninitialized grades:\n");
    test_uninitialized_grades();

    /* Test 6: String manipulation errors */
    printf("\n6. String manipulation:\n");
    string_manipulation_errors();

    return 0;
}