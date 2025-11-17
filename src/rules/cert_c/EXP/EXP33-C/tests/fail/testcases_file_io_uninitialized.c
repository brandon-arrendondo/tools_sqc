/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: file_io_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Uninitialized buffer in file operations */
void unsafe_file_operations(void) {
    FILE *file = fopen("test.txt", "r");
    char buffer[100];  /* Uninitialized */

    if (file) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            /* File read failed, buffer remains uninitialized */
        }
        fclose(file);
    }

    printf("Buffer: %s\n", buffer);  /* May print uninitialized data */
}

int main(void) {
    unsafe_file_operations();
    return 0;
}