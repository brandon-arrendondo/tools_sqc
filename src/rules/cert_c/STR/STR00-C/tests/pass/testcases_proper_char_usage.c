/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: proper_char_usage.c
 *
 * This case demonstrates compliant code that uses appropriate character
 * types according to STR00-C guidelines, ensuring portable and
 * consistent behavior across different platforms.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    /* COMPLIANT: Using plain char for string literals and basic character data */
    char greeting[] = "Hello, World!";
    char message[100];

    printf("Proper character type usage:\n");
    printf("Greeting: %s\n", greeting);

    /* COMPLIANT: String operations with consistent char types */
    strcpy(message, "Welcome to ");
    strcat(message, greeting);

    printf("Message: %s\n", message);

    /* COMPLIANT: Character manipulation with plain char */
    for (size_t i = 0; message[i] != '\0'; i++) {
        char c = message[i];
        if (c >= 'a' && c <= 'z') {
            message[i] = c - 32;  /* Convert to uppercase */
        }
    }

    printf("Uppercase: %s\n", message);

    /* COMPLIANT: String length and comparison operations */
    size_t len = strlen(greeting);
    printf("Greeting length: %zu\n", len);

    if (strcmp(greeting, "Hello, World!") == 0) {
        printf("Greeting matches expected value\n");
    }

    /* COMPLIANT: Character constants with appropriate types */
    char vowels[] = {'a', 'e', 'i', 'o', 'u', '\0'};
    char consonant = 'b';

    printf("Vowels: %s\n", vowels);
    printf("Consonant: %c\n", consonant);

    /* COMPLIANT: Character searching and manipulation */
    char *found = strchr(greeting, 'o');
    if (found != NULL) {
        printf("Found 'o' at position: %ld\n", found - greeting);
    }

    /* COMPLIANT: Proper character type for basic character set operations */
    char alphabet[27];
    for (int i = 0; i < 26; i++) {
        alphabet[i] = 'A' + i;
    }
    alphabet[26] = '\0';

    printf("Alphabet: %s\n", alphabet);

    /* COMPLIANT: String tokenization with char */
    char data[] = "apple,banana,cherry,date";
    char *token = strtok(data, ",");

    printf("Tokens: ");
    while (token != NULL) {
        printf("%s ", token);
        token = strtok(NULL, ",");
    }
    printf("\n");

    /* COMPLIANT: Character array initialization */
    char filename[] = "document.txt";
    char extension[5];

    /* Find file extension */
    char *dot = strrchr(filename, '.');
    if (dot != NULL) {
        strcpy(extension, dot + 1);
        printf("File extension: %s\n", extension);
    }

    /* COMPLIANT: Character buffer operations */
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Formatted string: %s (%zu chars)",
             greeting, strlen(greeting));

    printf("Buffer content: %s\n", buffer);

    return 0;
}