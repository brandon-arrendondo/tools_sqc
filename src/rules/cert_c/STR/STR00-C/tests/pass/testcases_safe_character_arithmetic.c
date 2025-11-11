/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: safe_character_arithmetic.c
 *
 * This case demonstrates compliant code that safely performs arithmetic
 * operations with character types, using appropriate types to avoid
 * overflow and sign-related issues.
 */

#include <stdio.h>
#include <limits.h>

int main(void) {
    printf("Safe character arithmetic operations:\n\n");

    /* COMPLIANT: Using int for character arithmetic */
    printf("Character range calculations:\n");

    /* Calculate alphabet range using int to avoid overflow */
    int start_char = 'A';
    int end_char = 'Z';
    int alphabet_range = end_char - start_char + 1;

    printf("Alphabet range: %c to %c = %d characters\n",
           (char)start_char, (char)end_char, alphabet_range);

    /* Calculate lowercase range */
    int lower_start = 'a';
    int lower_end = 'z';
    int lower_range = lower_end - lower_start + 1;

    printf("Lowercase range: %c to %c = %d characters\n",
           (char)lower_start, (char)lower_end, lower_range);

    /* Calculate digit range */
    int digit_start = '0';
    int digit_end = '9';
    int digit_range = digit_end - digit_start + 1;

    printf("Digit range: %c to %c = %d characters\n",
           (char)digit_start, (char)digit_end, digit_range);

    /* COMPLIANT: Safe character conversion with bounds checking */
    printf("\nSafe character conversion:\n");

    const char test_chars[] = "Hello123World";
    char converted[sizeof(test_chars)];

    for (size_t i = 0; test_chars[i] != '\0'; i++) {
        int ch = test_chars[i];

        /* Safe case conversion with bounds checking */
        if (ch >= 'a' && ch <= 'z') {
            /* Convert to uppercase, using int arithmetic */
            int upper_ch = ch - 'a' + 'A';
            converted[i] = (char)upper_ch;
        } else if (ch >= 'A' && ch <= 'Z') {
            /* Convert to lowercase, using int arithmetic */
            int lower_ch = ch - 'A' + 'a';
            converted[i] = (char)lower_ch;
        } else {
            /* Keep other characters unchanged */
            converted[i] = (char)ch;
        }
    }
    converted[sizeof(test_chars) - 1] = '\0';

    printf("Original: %s\n", test_chars);
    printf("Converted: %s\n", converted);

    /* COMPLIANT: Caesar cipher with safe arithmetic */
    printf("\nCaesar cipher (shift = 3):\n");

    const char plaintext[] = "HELLO WORLD";
    char ciphertext[sizeof(plaintext)];
    int shift = 3;

    for (size_t i = 0; plaintext[i] != '\0'; i++) {
        int ch = plaintext[i];

        if (ch >= 'A' && ch <= 'Z') {
            /* Safe modular arithmetic for alphabet wrapping */
            int shifted = ((ch - 'A' + shift) % 26) + 'A';
            ciphertext[i] = (char)shifted;
        } else {
            /* Keep non-alphabetic characters unchanged */
            ciphertext[i] = (char)ch;
        }
    }
    ciphertext[sizeof(plaintext) - 1] = '\0';

    printf("Plaintext:  %s\n", plaintext);
    printf("Ciphertext: %s\n", ciphertext);

    /* COMPLIANT: Reverse Caesar cipher */
    char decrypted[sizeof(ciphertext)];

    for (size_t i = 0; ciphertext[i] != '\0'; i++) {
        int ch = ciphertext[i];

        if (ch >= 'A' && ch <= 'Z') {
            /* Safe reverse modular arithmetic */
            int shifted = ((ch - 'A' - shift + 26) % 26) + 'A';
            decrypted[i] = (char)shifted;
        } else {
            decrypted[i] = (char)ch;
        }
    }
    decrypted[sizeof(ciphertext) - 1] = '\0';

    printf("Decrypted:  %s\n", decrypted);

    /* COMPLIANT: Character frequency analysis with int counters */
    printf("\nCharacter frequency analysis:\n");

    const char *sample_text = "The quick brown fox jumps over the lazy dog";
    int frequency[26] = {0};  /* Using int to avoid overflow */

    printf("Text: %s\n", sample_text);

    /* Count letter frequencies */
    for (size_t i = 0; sample_text[i] != '\0'; i++) {
        int ch = sample_text[i];

        /* Convert to uppercase and count */
        if (ch >= 'a' && ch <= 'z') {
            frequency[ch - 'a']++;
        } else if (ch >= 'A' && ch <= 'Z') {
            frequency[ch - 'A']++;
        }
    }

    /* Display frequencies */
    printf("Letter frequencies:\n");
    for (int i = 0; i < 26; i++) {
        if (frequency[i] > 0) {
            printf("%c: %d times\n", 'A' + i, frequency[i]);
        }
    }

    /* COMPLIANT: ROT13 encoding with safe arithmetic */
    printf("\nROT13 encoding:\n");

    char rot13_input[] = "Hello World";
    char rot13_output[sizeof(rot13_input)];

    for (size_t i = 0; rot13_input[i] != '\0'; i++) {
        int ch = rot13_input[i];

        if (ch >= 'A' && ch <= 'Z') {
            /* ROT13 for uppercase */
            int rotated = ((ch - 'A' + 13) % 26) + 'A';
            rot13_output[i] = (char)rotated;
        } else if (ch >= 'a' && ch <= 'z') {
            /* ROT13 for lowercase */
            int rotated = ((ch - 'a' + 13) % 26) + 'a';
            rot13_output[i] = (char)rotated;
        } else {
            /* Keep other characters unchanged */
            rot13_output[i] = (char)ch;
        }
    }
    rot13_output[sizeof(rot13_input) - 1] = '\0';

    printf("Original: %s\n", rot13_input);
    printf("ROT13:    %s\n", rot13_output);

    /* COMPLIANT: Hexadecimal digit conversion */
    printf("\nHexadecimal digit conversion:\n");

    const char hex_string[] = "1A2B3C4D";
    printf("Hex string: %s\n", hex_string);

    printf("Digit values:\n");
    for (size_t i = 0; hex_string[i] != '\0'; i++) {
        int ch = hex_string[i];
        int digit_value;

        /* Safe conversion with explicit bounds checking */
        if (ch >= '0' && ch <= '9') {
            digit_value = ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            digit_value = ch - 'A' + 10;
        } else if (ch >= 'a' && ch <= 'f') {
            digit_value = ch - 'a' + 10;
        } else {
            digit_value = -1;  /* Invalid hex digit */
        }

        if (digit_value >= 0) {
            printf("'%c' = %d\n", ch, digit_value);
        } else {
            printf("'%c' = invalid hex digit\n", ch);
        }
    }

    /* COMPLIANT: Safe character sequence generation */
    printf("\nCharacter sequence generation:\n");

    /* Generate alphabet with safe arithmetic */
    char alphabet[27];  /* 26 letters + null terminator */
    for (int i = 0; i < 26; i++) {
        alphabet[i] = (char)('A' + i);  /* Safe because i is bounded */
    }
    alphabet[26] = '\0';

    printf("Generated alphabet: %s\n", alphabet);

    /* Generate digits with safe arithmetic */
    char digits[11];  /* 10 digits + null terminator */
    for (int i = 0; i < 10; i++) {
        digits[i] = (char)('0' + i);  /* Safe because i is bounded */
    }
    digits[10] = '\0';

    printf("Generated digits: %s\n", digits);

    /* COMPLIANT: Character validation with safe comparisons */
    printf("\nCharacter validation:\n");

    const char test_input[] = "Test123!@#";
    int alpha_count = 0;
    int digit_count = 0;
    int special_count = 0;

    printf("Input: %s\n", test_input);

    for (size_t i = 0; test_input[i] != '\0'; i++) {
        int ch = test_input[i];

        /* Safe character classification */
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            alpha_count++;
        } else if (ch >= '0' && ch <= '9') {
            digit_count++;
        } else {
            special_count++;
        }
    }

    printf("Alphabetic: %d\n", alpha_count);
    printf("Digits: %d\n", digit_count);
    printf("Special: %d\n", special_count);

    /* COMPLIANT: Base conversion with safe arithmetic */
    printf("\nBase conversion:\n");

    const char base36_chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int number = 1234;

    printf("Converting %d to base 36:\n", number);

    /* Convert to base 36 string */
    char base36_result[32];
    int pos = 0;
    int temp = number;

    if (temp == 0) {
        base36_result[pos++] = '0';
    } else {
        while (temp > 0) {
            int remainder = temp % 36;
            base36_result[pos++] = base36_chars[remainder];  /* Safe array access */
            temp /= 36;
        }
    }

    /* Reverse the string */
    for (int i = 0; i < pos / 2; i++) {
        char swap = base36_result[i];
        base36_result[i] = base36_result[pos - 1 - i];
        base36_result[pos - 1 - i] = swap;
    }

    base36_result[pos] = '\0';
    printf("Result: %s\n", base36_result);

    return 0;
}