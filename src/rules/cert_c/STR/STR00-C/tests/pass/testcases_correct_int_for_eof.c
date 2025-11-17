/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: correct_int_for_eof.c
 *
 * This case demonstrates compliant code that uses int type for
 * character input operations to properly handle EOF values,
 * following STR00-C guidelines.
 */

#include <stdio.h>

int main(void) {
    /* COMPLIANT: Using int for character input to handle EOF */
    int c;  /* Correct type to hold both characters and EOF */

    printf("Character input handling with proper types:\n");
    printf("Enter characters (Ctrl+D or Ctrl+Z to end):\n");

    /* COMPLIANT: Proper EOF detection with int type */
    while ((c = getchar()) != EOF) {
        if (c == 'q' || c == 'Q') {
            printf("Quit character entered\n");
            break;
        }

        printf("Read character: ");
        if (c >= 32 && c <= 126) {
            printf("'%c' (ASCII: %d)\n", c, c);
        } else if (c == '\n') {
            printf("newline\n");
        } else if (c == '\t') {
            printf("tab\n");
        } else {
            printf("control character (ASCII: %d)\n", c);
        }
    }

    /* COMPLIANT: Proper EOF detection and handling */
    if (c == EOF) {
        printf("End of input reached (EOF)\n");
    }

    /* Clear any remaining input */
    while (getchar() != '\n' && !feof(stdin));

    /* COMPLIANT: File reading with proper character type */
    FILE *file = fopen(__FILE__, "r");  /* Read this source file */
    if (file != NULL) {
        printf("\nReading file content:\n");
        int line_count = 1;
        int char_count = 0;

        /* COMPLIANT: Use int for file character operations */
        while ((c = fgetc(file)) != EOF) {
            char_count++;

            if (c == '\n') {
                line_count++;
            }

            /* Display first 100 characters */
            if (char_count <= 100) {
                if (c >= 32 && c <= 126) {
                    putchar(c);
                } else if (c == '\n') {
                    putchar('\n');
                } else {
                    putchar('.');
                }
            }
        }

        /* COMPLIANT: Check for EOF vs error conditions */
        if (feof(file)) {
            printf("\n\nFile read completed successfully\n");
        } else if (ferror(file)) {
            printf("\n\nFile read error occurred\n");
        }

        printf("Total lines: %d, Total characters: %d\n", line_count, char_count);
        fclose(file);
    }

    /* COMPLIANT: Character classification with proper int usage */
    printf("\nCharacter classification test:\n");

    const char test_string[] = "Hello123!@#";
    for (size_t i = 0; test_string[i] != '\0'; i++) {
        /* COMPLIANT: Cast to unsigned char before passing to ctype functions */
        int ch = (unsigned char)test_string[i];

        printf("Character '%c': ", test_string[i]);

        if (isalpha(ch)) {
            printf("alphabetic ");
        }
        if (isdigit(ch)) {
            printf("digit ");
        }
        if (ispunct(ch)) {
            printf("punctuation ");
        }
        if (isprint(ch)) {
            printf("printable ");
        }

        printf("\n");
    }

    /* COMPLIANT: Case conversion with proper types */
    printf("\nCase conversion:\n");

    char mixed_case[] = "MiXeD CaSe TeXt";
    printf("Original: %s\n", mixed_case);

    for (size_t i = 0; mixed_case[i] != '\0'; i++) {
        /* COMPLIANT: Proper use of ctype functions with casting */
        int ch = (unsigned char)mixed_case[i];

        if (islower(ch)) {
            mixed_case[i] = (char)toupper(ch);  /* Safe cast after validation */
        } else if (isupper(ch)) {
            mixed_case[i] = (char)tolower(ch);  /* Safe cast after validation */
        }
    }

    printf("Converted: %s\n", mixed_case);

    /* COMPLIANT: Character input validation */
    printf("\nEnter a single character for validation: ");
    c = getchar();

    if (c != EOF) {
        printf("You entered: ");

        if (c >= 32 && c <= 126) {
            printf("'%c'\n", c);
        } else {
            printf("a control character (code: %d)\n", c);
        }

        /* COMPLIANT: Character analysis with proper casting */
        int ch = (unsigned char)c;
        printf("Character properties:\n");
        printf("  Alphabetic: %s\n", isalpha(ch) ? "yes" : "no");
        printf("  Numeric: %s\n", isdigit(ch) ? "yes" : "no");
        printf("  Alphanumeric: %s\n", isalnum(ch) ? "yes" : "no");
        printf("  Printable: %s\n", isprint(ch) ? "yes" : "no");
        printf("  Whitespace: %s\n", isspace(ch) ? "yes" : "no");
    }

    /* Clear input buffer */
    while (getchar() != '\n' && !feof(stdin));

    return 0;
}