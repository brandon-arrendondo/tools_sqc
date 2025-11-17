/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: mixed_char_types.c
 *
 * This case demonstrates a violation of STR00-C by mixing different
 * character types inconsistently, leading to type compatibility
 * issues and potential compilation warnings.
 */

#include <stdio.h>
#include <string.h>

void process_string(char *str) {
    printf("Processing: %s\n", str);
}

int main(void) {
    /* VIOLATION: Mixing different character types inconsistently */
    char plain_string[] = "Plain char string";
    signed char signed_string[] = "Signed char string";
    unsigned char unsigned_string[] = "Unsigned char string";

    printf("Original strings:\n");
    printf("Plain: %s\n", plain_string);
    printf("Signed: %s\n", signed_string);      /* Warning */
    printf("Unsigned: %s\n", unsigned_string);  /* Warning */

    /* VIOLATION: Passing different char types to same function */
    process_string(plain_string);                /* OK */
    process_string(signed_string);               /* Warning */
    process_string(unsigned_string);             /* Warning */

    /* VIOLATION: String comparison with mixed types */
    if (strcmp(plain_string, signed_string) == 0) {     /* Warning */
        printf("Plain and signed match\n");
    }

    if (strcmp(signed_string, unsigned_string) == 0) {  /* Warning */
        printf("Signed and unsigned match\n");
    }

    /* VIOLATION: String copying between different char types */
    char dest1[50];
    signed char dest2[50];
    unsigned char dest3[50];

    strcpy(dest1, plain_string);      /* OK */
    strcpy(dest2, signed_string);     /* Warning */
    strcpy(dest3, unsigned_string);   /* Warning */

    /* Cross-type copying - more warnings */
    strcpy(dest1, signed_string);     /* Warning */
    strcpy(dest2, unsigned_string);   /* Warning */
    strcpy(dest3, plain_string);      /* Warning */

    /* VIOLATION: Character-by-character operations with mixed types */
    for (size_t i = 0; i < strlen(plain_string); i++) {
        signed_string[i] = plain_string[i];      /* Warning */
        unsigned_string[i] = signed_string[i];   /* Warning */
    }

    /* VIOLATION: Pointer arithmetic with mixed char types */
    char *p1 = plain_string;
    signed char *p2 = signed_string;
    unsigned char *p3 = unsigned_string;

    /* Type mismatch assignments */
    p1 = p2;  /* Warning */
    p2 = p3;  /* Warning */
    p3 = p1;  /* Warning */

    /* VIOLATION: Function pointer with different char types */
    size_t (*strlen_func)(const char *) = strlen;
    size_t len1 = strlen_func(signed_string);    /* Warning */
    size_t len2 = strlen_func(unsigned_string);  /* Warning */

    printf("Lengths: %zu, %zu\n", len1, len2);

    return 0;
}