/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: array_indexing_char.c
 *
 * This case demonstrates a violation of STR00-C by using plain char
 * for array indexing operations where the signedness of char can
 * lead to undefined behavior with negative indices.
 */

#include <stdio.h>

int main(void) {
    int lookup_table[256];
    char frequency_table[128];

    /* Initialize tables */
    for (int i = 0; i < 256; i++) {
        lookup_table[i] = i * 2;
    }

    for (int i = 0; i < 128; i++) {
        frequency_table[i] = 0;
    }

    /* VIOLATION: Using char as array index */
    char test_chars[] = {100, 150, 200, 250, -1, -50};  /* Some may be negative */

    printf("Array indexing with char values:\n");
    for (size_t i = 0; i < sizeof(test_chars); i++) {
        char index = test_chars[i];

        printf("Index value: %d (as char)\n", index);

        /* VIOLATION: Potential negative array index */
        if (index >= 0 && index < 256) {
            printf("  lookup_table[%d] = %d\n", index, lookup_table[index]);
        } else {
            printf("  Index out of bounds\n");
        }

        /* VIOLATION: Direct use without bounds check */
        /* This could cause undefined behavior if index is negative */
        int value = lookup_table[index];  /* Undefined if index < 0 */
        printf("  Direct access result: %d\n", value);
    }

    /* VIOLATION: Character codes as array indices */
    const char *text = "Hello World!";
    int char_counts[256] = {0};

    printf("\nCharacter frequency counting:\n");
    for (size_t i = 0; text[i] != '\0'; i++) {
        char c = text[i];

        /* VIOLATION: Using char directly as array index */
        /* If char is signed and contains high-bit characters, undefined behavior */
        char_counts[c]++;  /* Potential negative index */

        printf("Character '%c' (value: %d) count: %d\n",
               c, c, char_counts[c]);
    }

    /* VIOLATION: Extended ASCII characters as indices */
    char extended_text[] = {72, 101, 108, 108, 111, 128, 150, 200, 255, 0};

    printf("\nExtended ASCII indexing:\n");
    for (size_t i = 0; extended_text[i] != '\0'; i++) {
        char index = extended_text[i];

        printf("Character code: %d\n", index);

        /* VIOLATION: May access negative array indices */
        char_counts[index]++;  /* Undefined behavior if index < 0 */
    }

    /* VIOLATION: Calculation resulting in char used as index */
    char base = 'A';
    char offset = 50;  /* This could make (base + offset) negative on some systems */

    char calculated_index = base + offset;
    printf("\nCalculated index: %d\n", calculated_index);

    /* VIOLATION: Using calculated char as index */
    if (calculated_index >= 0 && calculated_index < 256) {
        printf("Value at calculated index: %d\n", lookup_table[calculated_index]);
    }

    /* Direct use without check - dangerous */
    int dangerous_value = lookup_table[calculated_index];  /* Potential undefined behavior */

    /* VIOLATION: Character arithmetic for indexing */
    char start_char = 'z';
    for (char c = start_char; c >= 'a'; c--) {  /* c may wrap to positive */
        /* VIOLATION: Using character directly as index */
        frequency_table[c]++;  /* Undefined if c becomes negative */

        if (c == 'a') break;  /* Prevent infinite loop if char is unsigned */
    }

    /* VIOLATION: Function parameter as index */
    char param_index = (char)getchar();
    while (getchar() != '\n');  /* Clear buffer */

    printf("User input as index: %d\n", param_index);
    /* VIOLATION: Direct use of user input as index */
    printf("Table value: %d\n", lookup_table[param_index]);  /* Dangerous */

    return 0;
}