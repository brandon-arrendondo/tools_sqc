/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: char_vs_int_eof.c
 *
 * This case demonstrates a violation of STR00-C by using char type
 * for character input operations where int would be appropriate
 * to properly handle EOF values.
 */

#include <stdio.h>

int main(void) {
    /* VIOLATION: Using char for getchar() return value */
    char c;  /* Should be int to handle EOF */

    printf("Enter characters (Ctrl+D to end):\n");

    /* VIOLATION: Cannot properly detect EOF */
    while ((c = getchar()) != EOF) {  /* Problem: char cannot represent EOF */
        if (c == 'q') {
            break;
        }
        printf("Read character: %c (ASCII: %d)\n", c, (int)c);
    }

    /* VIOLATION: EOF detection may fail */
    if (c == EOF) {
        printf("EOF detected\n");  /* May never be reached */
    } else {
        printf("Loop ended by 'q' character\n");
    }

    /* VIOLATION: File reading with wrong character type */
    FILE *file = fopen(__FILE__, "r");
    if (file != NULL) {
        char file_char;  /* Should be int */

        printf("\nFirst 10 characters from file:\n");
        for (int i = 0; i < 10; i++) {
            file_char = fgetc(file);  /* Cannot detect EOF properly */
            if (file_char == EOF) {   /* Comparison may fail */
                printf("Premature EOF detected\n");
                break;
            }
            printf("%c", file_char);
        }
        printf("\n");

        fclose(file);
    }

    /* VIOLATION: Character comparison in loop */
    printf("\nReading until newline:\n");
    char input_char;
    while ((input_char = getchar()) != '\n') {  /* Should use int */
        if (input_char == EOF) {  /* May not work correctly */
            printf("EOF reached\n");
            break;
        }
        putchar(input_char);
    }

    /* VIOLATION: Using char for character functions that return int */
    char test_char = 'a';
    char upper_char = toupper(test_char);  /* toupper returns int */
    char lower_char = tolower(test_char);  /* tolower returns int */

    printf("Original: %c, Upper: %c, Lower: %c\n",
           test_char, upper_char, lower_char);

    return 0;
}