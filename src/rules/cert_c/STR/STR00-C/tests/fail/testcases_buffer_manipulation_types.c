/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: buffer_manipulation_types.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for buffer manipulation operations, leading to
 * type mismatches and potential data corruption issues.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    /* VIOLATION: Buffer allocation with different character types */
    signed char *signed_buffer = malloc(100 * sizeof(signed char));
    unsigned char *unsigned_buffer = malloc(100 * sizeof(unsigned char));
    char *plain_buffer = malloc(100 * sizeof(char));

    if (!signed_buffer || !unsigned_buffer || !plain_buffer) {
        printf("Memory allocation failed\n");
        return 1;
    }

    /* VIOLATION: Buffer initialization with type mismatches */
    const char *init_data = "Initial buffer data with some \x80\x90\xFF bytes";

    strcpy(signed_buffer, init_data);    /* Warning */
    strcpy(unsigned_buffer, init_data);  /* Warning */
    strcpy(plain_buffer, init_data);     /* OK */

    printf("Buffer initialization with different types:\n");
    printf("Signed buffer: %s\n", signed_buffer);    /* Warning */
    printf("Unsigned buffer: %s\n", unsigned_buffer); /* Warning */
    printf("Plain buffer: %s\n", plain_buffer);

    /* VIOLATION: Buffer copying between different types */
    signed char source_signed[] = "Source from signed char";
    unsigned char source_unsigned[] = "Source from unsigned char";

    /* Cross-type copying */
    strcpy(plain_buffer, source_signed);     /* Warning */
    strcpy(signed_buffer, source_unsigned);  /* Warning */
    strcpy(unsigned_buffer, plain_buffer);   /* Warning */

    printf("\nAfter cross-type copying:\n");
    printf("Plain (from signed): %s\n", plain_buffer);
    printf("Signed (from unsigned): %s\n", signed_buffer);    /* Warning */
    printf("Unsigned (from plain): %s\n", unsigned_buffer);   /* Warning */

    /* VIOLATION: Buffer concatenation with type issues */
    char append_data[] = " - appended";
    strcat(signed_buffer, append_data);    /* Warning */
    strcat(unsigned_buffer, append_data);  /* Warning */

    printf("\nAfter concatenation:\n");
    printf("Signed buffer: %s\n", signed_buffer);    /* Warning */
    printf("Unsigned buffer: %s\n", unsigned_buffer); /* Warning */

    /* VIOLATION: Buffer searching with wrong types */
    signed char search_char = 'a';
    unsigned char *found_unsigned = strchr(unsigned_buffer, search_char);  /* Warning */
    signed char *found_signed = strchr(signed_buffer, search_char);        /* Warning */

    printf("\nBuffer search results:\n");
    if (found_unsigned) {
        printf("Found in unsigned buffer at position: %ld\n",
               found_unsigned - unsigned_buffer);
    }
    if (found_signed) {
        printf("Found in signed buffer at position: %ld\n",
               found_signed - signed_buffer);
    }

    /* VIOLATION: Buffer comparison with mixed types */
    int cmp_result = strcmp(signed_buffer, unsigned_buffer);  /* Warning */
    printf("Buffer comparison result: %d\n", cmp_result);

    /* VIOLATION: Manual buffer operations with type issues */
    printf("\nManual buffer operations:\n");

    /* Character-by-character copy with type conversion */
    for (size_t i = 0; i < 10 && signed_buffer[i] != '\0'; i++) {
        unsigned_buffer[i] = signed_buffer[i];  /* Implicit conversion */
        plain_buffer[i] = unsigned_buffer[i];   /* Implicit conversion */
    }

    /* VIOLATION: Buffer reversal with character type issues */
    size_t len = strlen(plain_buffer);
    for (size_t i = 0; i < len / 2; i++) {
        /* Swap characters with temporary variables of wrong types */
        signed char temp_signed = plain_buffer[i];           /* Conversion */
        plain_buffer[i] = plain_buffer[len - 1 - i];
        plain_buffer[len - 1 - i] = temp_signed;             /* Conversion */
    }

    printf("Reversed buffer: %s\n", plain_buffer);

    /* VIOLATION: Buffer formatting with type mismatches */
    signed char format_buffer[200];
    sprintf(format_buffer, "Formatted: %s + %s",            /* Warning */
            signed_buffer, unsigned_buffer);                 /* Warning */

    printf("Formatted result: %s\n", format_buffer);        /* Warning */

    /* VIOLATION: Buffer tokenization with type issues */
    unsigned char tokenize_data[] = "token1,token2,token3,token4";
    unsigned char *token = strtok(tokenize_data, ",");       /* Warning */

    printf("\nTokenization with unsigned char:\n");
    int token_count = 0;
    while (token != NULL) {
        printf("Token %d: %s\n", ++token_count, token);     /* Warning */
        token = strtok(NULL, ",");                           /* Warning */
    }

    /* VIOLATION: Buffer validation with character type issues */
    printf("\nBuffer validation:\n");

    /* Check for valid ASCII in different buffer types */
    int valid_ascii_count = 0;
    for (size_t i = 0; signed_buffer[i] != '\0'; i++) {
        signed char c = signed_buffer[i];
        if (c >= 0 && c < 128) {  /* ASCII range check with sign dependency */
            valid_ascii_count++;
        }
    }
    printf("Valid ASCII characters in signed buffer: %d\n", valid_ascii_count);

    /* VIOLATION: Buffer encryption/decryption simulation */
    unsigned char encrypt_buffer[100];
    strcpy(encrypt_buffer, "Data to encrypt");               /* Warning */

    printf("\nBuffer encryption simulation:\n");
    unsigned char key = 0x5A;

    /* Simple XOR encryption */
    for (size_t i = 0; encrypt_buffer[i] != '\0'; i++) {
        encrypt_buffer[i] ^= key;
    }

    printf("Encrypted buffer (hex): ");
    for (size_t i = 0; encrypt_buffer[i] != '\0'; i++) {
        printf("%02X ", encrypt_buffer[i]);
    }
    printf("\n");

    /* Decrypt back */
    for (size_t i = 0; encrypt_buffer[i] != '\0'; i++) {
        encrypt_buffer[i] ^= key;
    }
    printf("Decrypted buffer: %s\n", encrypt_buffer);        /* Warning */

    /* VIOLATION: Buffer resizing with type confusion */
    signed_buffer = realloc(signed_buffer, 200);
    if (signed_buffer == NULL) {
        printf("Realloc failed\n");
        goto cleanup;
    }

    /* Fill extended space with pattern */
    for (size_t i = strlen((char*)signed_buffer); i < 150; i++) {
        signed_buffer[i] = 'X';
    }
    signed_buffer[150] = '\0';

    printf("\nResized and filled buffer length: %zu\n",
           strlen((char*)signed_buffer));

    /* VIOLATION: Buffer boundary operations */
    char boundary_test[10] = "123456789";  /* Exactly fills buffer */
    signed char *signed_boundary = (signed char*)boundary_test;

    /* This operation is at the boundary */
    signed_boundary[9] = '\0';  /* Ensure null termination */
    printf("Boundary buffer: %s\n", signed_boundary);       /* Warning */

cleanup:
    /* Cleanup */
    free(signed_buffer);
    free(unsigned_buffer);
    free(plain_buffer);

    return 0;
}