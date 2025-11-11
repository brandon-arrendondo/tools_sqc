/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: int_for_string_operations.c
 *
 * This case demonstrates a violation of STR00-C by using int type
 * for string operations where char would be more appropriate,
 * leading to inefficient memory usage and potential issues.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    /* VIOLATION: Using int array for string storage */
    int int_string[20];
    const char *source = "Hello";

    /* Copy string using inappropriate int storage */
    for (int i = 0; source[i] != '\0'; i++) {
        int_string[i] = source[i];  /* Waste of memory - 4 bytes per char */
    }
    int_string[strlen(source)] = '\0';

    printf("String stored as int array:\n");
    for (int i = 0; int_string[i] != 0; i++) {
        printf("%c", (char)int_string[i]);
    }
    printf("\n");

    /* VIOLATION: String manipulation with int arrays */
    for (int i = 0; int_string[i] != 0; i++) {
        if (int_string[i] >= 'a' && int_string[i] <= 'z') {
            int_string[i] = int_string[i] - 32;  /* Convert to uppercase */
        }
    }

    /* VIOLATION: Inappropriate memory usage comparison */
    printf("Memory usage:\n");
    printf("char array for \"%s\": %zu bytes\n", source, strlen(source) + 1);
    printf("int array for same string: %zu bytes\n",
           (strlen(source) + 1) * sizeof(int));

    /* VIOLATION: Function parameters expecting char but using int */
    int single_char = 'A';
    printf("Character: %c\n", single_char);  /* Inefficient storage */

    /* VIOLATION: Character classification with wrong type */
    int test_chars[] = {'a', 'B', '1', '!', '\0'};
    for (int i = 0; test_chars[i] != 0; i++) {
        printf("Character %c is ", (char)test_chars[i]);
        if (test_chars[i] >= '0' && test_chars[i] <= '9') {
            printf("a digit\n");
        } else if (test_chars[i] >= 'A' && test_chars[i] <= 'Z') {
            printf("uppercase\n");
        } else if (test_chars[i] >= 'a' && test_chars[i] <= 'z') {
            printf("lowercase\n");
        } else {
            printf("other\n");
        }
    }

    return 0;
}