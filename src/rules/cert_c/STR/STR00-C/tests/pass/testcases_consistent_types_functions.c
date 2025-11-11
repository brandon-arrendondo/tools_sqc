/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: consistent_types_functions.c
 *
 * This case demonstrates compliant code that uses consistent character
 * types in function parameters and return values, avoiding type
 * compatibility issues and ensuring clean compilation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* COMPLIANT: Function using char* for string operations */
size_t count_characters(const char *str, char target) {
    size_t count = 0;

    if (str == NULL) return 0;

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == target) {
            count++;
        }
    }

    return count;
}

/* COMPLIANT: Function for string manipulation with consistent types */
char *string_to_uppercase(char *str) {
    if (str == NULL) return NULL;

    for (size_t i = 0; str[i] != '\0'; i++) {
        /* COMPLIANT: Proper casting for ctype functions */
        int ch = (unsigned char)str[i];
        if (islower(ch)) {
            str[i] = (char)toupper(ch);
        }
    }

    return str;
}

/* COMPLIANT: Function for string creation with proper memory management */
char *create_greeting(const char *name) {
    if (name == NULL) return NULL;

    const char *prefix = "Hello, ";
    const char *suffix = "!";

    size_t total_length = strlen(prefix) + strlen(name) + strlen(suffix) + 1;
    char *greeting = malloc(total_length);

    if (greeting != NULL) {
        strcpy(greeting, prefix);
        strcat(greeting, name);
        strcat(greeting, suffix);
    }

    return greeting;
}

/* COMPLIANT: Function using unsigned char for byte operations */
unsigned char calculate_checksum(const unsigned char *data, size_t length) {
    unsigned char checksum = 0;

    if (data == NULL) return 0;

    for (size_t i = 0; i < length; i++) {
        checksum += data[i];
    }

    return checksum;
}

/* COMPLIANT: Function for string comparison with proper types */
int compare_strings_case_insensitive(const char *str1, const char *str2) {
    if (str1 == NULL && str2 == NULL) return 0;
    if (str1 == NULL) return -1;
    if (str2 == NULL) return 1;

    while (*str1 && *str2) {
        /* COMPLIANT: Proper character comparison with casting */
        int c1 = tolower((unsigned char)*str1);
        int c2 = tolower((unsigned char)*str2);

        if (c1 != c2) {
            return c1 - c2;
        }

        str1++;
        str2++;
    }

    return (unsigned char)*str1 - (unsigned char)*str2;
}

/* COMPLIANT: Function for character validation */
int is_valid_identifier(const char *str) {
    if (str == NULL || *str == '\0') return 0;

    /* First character must be letter or underscore */
    int first_char = (unsigned char)*str;
    if (!isalpha(first_char) && first_char != '_') {
        return 0;
    }

    /* Remaining characters must be alphanumeric or underscore */
    for (size_t i = 1; str[i] != '\0'; i++) {
        int ch = (unsigned char)str[i];
        if (!isalnum(ch) && ch != '_') {
            return 0;
        }
    }

    return 1;
}

/* COMPLIANT: Function for string tokenization */
char **split_string(const char *str, char delimiter, size_t *count) {
    if (str == NULL || count == NULL) return NULL;

    *count = 0;

    /* Count delimiters to determine array size */
    size_t delimiter_count = 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == delimiter) {
            delimiter_count++;
        }
    }

    size_t token_count = delimiter_count + 1;
    char **tokens = malloc(token_count * sizeof(char*));
    if (tokens == NULL) return NULL;

    /* Create working copy of string */
    char *str_copy = malloc(strlen(str) + 1);
    if (str_copy == NULL) {
        free(tokens);
        return NULL;
    }
    strcpy(str_copy, str);

    /* Tokenize */
    char delim_str[2] = {delimiter, '\0'};
    char *token = strtok(str_copy, delim_str);
    size_t index = 0;

    while (token != NULL && index < token_count) {
        tokens[index] = malloc(strlen(token) + 1);
        if (tokens[index] != NULL) {
            strcpy(tokens[index], token);
            index++;
        }
        token = strtok(NULL, delim_str);
    }

    *count = index;
    free(str_copy);

    return tokens;
}

/* COMPLIANT: Function to free token array */
void free_tokens(char **tokens, size_t count) {
    if (tokens == NULL) return;

    for (size_t i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

int main(void) {
    printf("Consistent character types in functions:\n\n");

    /* COMPLIANT: Using functions with consistent char types */
    char text[] = "Hello, World! This is a test string.";

    printf("Original text: %s\n", text);

    /* Count specific characters */
    size_t o_count = count_characters(text, 'o');
    size_t space_count = count_characters(text, ' ');

    printf("Letter 'o' appears %zu times\n", o_count);
    printf("Spaces appear %zu times\n", space_count);

    /* Create and use greeting */
    char *greeting = create_greeting("Alice");
    if (greeting != NULL) {
        printf("Greeting: %s\n", greeting);

        /* Convert to uppercase */
        string_to_uppercase(greeting);
        printf("Uppercase: %s\n", greeting);

        free(greeting);
    }

    /* COMPLIANT: String comparison */
    const char *str1 = "Hello";
    const char *str2 = "HELLO";
    const char *str3 = "World";

    printf("\nString comparisons:\n");
    printf("'%s' vs '%s' (case-insensitive): %d\n",
           str1, str2, compare_strings_case_insensitive(str1, str2));
    printf("'%s' vs '%s' (case-insensitive): %d\n",
           str1, str3, compare_strings_case_insensitive(str1, str3));

    /* COMPLIANT: Identifier validation */
    const char *identifiers[] = {
        "valid_identifier",
        "_private_var",
        "CamelCase123",
        "123invalid",
        "invalid-name",
        "another_valid_one"
    };

    printf("\nIdentifier validation:\n");
    for (size_t i = 0; i < 6; i++) {
        printf("'%s': %s\n", identifiers[i],
               is_valid_identifier(identifiers[i]) ? "valid" : "invalid");
    }

    /* COMPLIANT: Binary data operations with unsigned char */
    unsigned char binary_data[] = {
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64
    };

    unsigned char checksum = calculate_checksum(binary_data, sizeof(binary_data));

    printf("\nBinary data checksum: 0x%02X\n", checksum);
    printf("Binary data as string: %s\n", (char*)binary_data);

    /* COMPLIANT: String tokenization */
    const char *csv_data = "apple,banana,cherry,date,elderberry";
    size_t token_count;
    char **tokens = split_string(csv_data, ',', &token_count);

    if (tokens != NULL) {
        printf("\nTokenized CSV data:\n");
        for (size_t i = 0; i < token_count; i++) {
            printf("Token %zu: %s\n", i + 1, tokens[i]);
        }

        free_tokens(tokens, token_count);
    }

    /* COMPLIANT: Character classification with proper casting */
    const char *test_string = "Test123!@#";

    printf("\nCharacter analysis of '%s':\n", test_string);

    int alpha_count = 0, digit_count = 0, punct_count = 0;

    for (size_t i = 0; test_string[i] != '\0'; i++) {
        int ch = (unsigned char)test_string[i];

        if (isalpha(ch)) alpha_count++;
        else if (isdigit(ch)) digit_count++;
        else if (ispunct(ch)) punct_count++;
    }

    printf("Alphabetic characters: %d\n", alpha_count);
    printf("Digit characters: %d\n", digit_count);
    printf("Punctuation characters: %d\n", punct_count);

    /* COMPLIANT: File path operations */
    const char *file_path = "/home/user/documents/readme.txt";

    printf("\nFile path analysis:\n");
    printf("Full path: %s\n", file_path);

    /* Find filename (last component after /) */
    const char *filename = strrchr(file_path, '/');
    if (filename != NULL) {
        filename++;  /* Skip the '/' */
        printf("Filename: %s\n", filename);

        /* Find extension */
        const char *extension = strrchr(filename, '.');
        if (extension != NULL) {
            printf("Extension: %s\n", extension);
        }
    }

    return 0;
}