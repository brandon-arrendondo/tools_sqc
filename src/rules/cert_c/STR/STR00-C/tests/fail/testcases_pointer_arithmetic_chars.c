/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: pointer_arithmetic_chars.c
 *
 * This case demonstrates a violation of STR00-C by performing pointer
 * arithmetic with inappropriate character types, leading to type
 * compatibility issues and potential undefined behavior.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    /* VIOLATION: Mixing different character pointer types */
    char plain_string[] = "Plain string";
    signed char signed_string[] = "Signed string";
    unsigned char unsigned_string[] = "Unsigned string";

    char *plain_ptr = plain_string;
    signed char *signed_ptr = signed_string;
    unsigned char *unsigned_ptr = unsigned_string;

    printf("Pointer arithmetic with different character types:\n");

    /* VIOLATION: Cross-assignment of different character pointer types */
    plain_ptr = signed_ptr;      /* Warning: incompatible pointer types */
    signed_ptr = unsigned_ptr;   /* Warning: incompatible pointer types */
    unsigned_ptr = plain_ptr;    /* Warning: incompatible pointer types */

    /* VIOLATION: Pointer arithmetic with type mismatches */
    printf("Pointer positions:\n");
    printf("Plain ptr position: %ld\n", plain_ptr - plain_string);
    printf("Signed ptr position: %ld\n", signed_ptr - signed_string);
    printf("Unsigned ptr position: %ld\n", unsigned_ptr - unsigned_string);

    /* VIOLATION: Comparison between different character pointer types */
    if (plain_ptr == signed_ptr) {      /* Warning: comparison of different types */
        printf("Pointers are equal\n");
    }

    if (signed_ptr < unsigned_ptr) {    /* Warning: comparison of different types */
        printf("Signed pointer is less than unsigned\n");
    }

    /* VIOLATION: Function calls with wrong pointer types */
    size_t len1 = strlen(signed_ptr);    /* Warning: incompatible pointer type */
    size_t len2 = strlen(unsigned_ptr);  /* Warning: incompatible pointer type */

    printf("String lengths: %zu, %zu\n", len1, len2);

    /* VIOLATION: Pointer increment/decrement with type issues */
    char *search_ptr = strchr(signed_string, 'S');  /* Warning */
    if (search_ptr != NULL) {
        search_ptr++;  /* Moving in wrong pointer type context */
        printf("Character after 'S': %c\n", *search_ptr);
    }

    /* VIOLATION: Array access through wrong pointer types */
    signed char *array_ptr = (signed char*)plain_string;  /* Explicit cast */

    for (int i = 0; i < 5; i++) {
        printf("array_ptr[%d] = %c (%d)\n", i, array_ptr[i], array_ptr[i]);
        array_ptr++;  /* Incrementing signed char pointer */
    }

    /* VIOLATION: Pointer subtraction between different types */
    char *start = plain_string;
    signed char *end = signed_string + strlen((char*)signed_string);

    /* This comparison/subtraction involves different pointer types */
    ptrdiff_t diff = (char*)end - start;  /* Warning and logical error */
    printf("Pointer difference: %ld\n", diff);

    /* VIOLATION: Memory operations with wrong pointer types */
    char destination[100];

    /* memcpy with type mismatches */
    memcpy(destination, signed_string, strlen((char*)signed_string));    /* Warning */
    memcpy(destination, unsigned_string, strlen((char*)unsigned_string)); /* Warning */

    /* VIOLATION: String manipulation with mixed pointers */
    char *token_ptr = strtok(signed_string, " ");  /* Warning */
    while (token_ptr != NULL) {
        printf("Token: %s\n", token_ptr);
        token_ptr = strtok(NULL, " ");
    }

    /* VIOLATION: Buffer operations with character type confusion */
    unsigned char buffer[50];
    signed char source[] = "Source data";

    /* Copy with type mismatch */
    for (size_t i = 0; i < strlen((char*)source); i++) {
        buffer[i] = source[i];  /* Type conversion warning */
    }
    buffer[strlen((char*)source)] = '\0';

    printf("Copied buffer: %s\n", (char*)buffer);

    /* VIOLATION: Pointer to different character types in structures */
    struct mixed_pointers {
        char *plain_ptr;
        signed char *signed_ptr;
        unsigned char *unsigned_ptr;
    };

    struct mixed_pointers mp = {
        .plain_ptr = signed_string,      /* Warning */
        .signed_ptr = unsigned_string,   /* Warning */
        .unsigned_ptr = plain_string     /* Warning */
    };

    printf("Structure pointer access:\n");
    printf("Plain via struct: %s\n", mp.plain_ptr);      /* Warning */
    printf("Signed via struct: %s\n", mp.signed_ptr);    /* Warning */
    printf("Unsigned via struct: %s\n", mp.unsigned_ptr); /* Warning */

    return 0;
}