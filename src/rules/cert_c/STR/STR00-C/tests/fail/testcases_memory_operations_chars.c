/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: memory_operations_chars.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types with memory operations, leading to type mismatches
 * and potential data handling issues.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    /* VIOLATION: Memory allocation for different character types */
    signed char *signed_memory = malloc(100 * sizeof(signed char));
    unsigned char *unsigned_memory = malloc(100 * sizeof(unsigned char));
    char *plain_memory = malloc(100 * sizeof(char));

    if (!signed_memory || !unsigned_memory || !plain_memory) {
        printf("Memory allocation failed\n");
        return 1;
    }

    /* VIOLATION: memset with different character types */
    memset(signed_memory, 0xFF, 100);     /* May set to -1 on signed char systems */
    memset(unsigned_memory, 0xFF, 100);   /* Sets to 255 */
    memset(plain_memory, 0xFF, 100);      /* Sign-dependent */

    printf("Memory initialization comparison:\n");
    printf("Signed char[0]: %d\n", signed_memory[0]);
    printf("Unsigned char[0]: %d\n", unsigned_memory[0]);
    printf("Plain char[0]: %d\n", plain_memory[0]);

    /* VIOLATION: memcpy between different character types */
    const char source[] = "Test data with \x80\x90\xA0\xFF";

    memcpy(signed_memory, source, strlen(source) + 1);    /* Warning */
    memcpy(unsigned_memory, source, strlen(source) + 1);  /* Warning */

    printf("\nAfter memcpy from plain char source:\n");
    for (size_t i = 0; i < 10; i++) {
        printf("Position %zu: signed=%d, unsigned=%d, plain=%d\n",
               i, signed_memory[i], unsigned_memory[i], plain_memory[i]);
    }

    /* VIOLATION: memcmp with mixed character types */
    int cmp1 = memcmp(signed_memory, unsigned_memory, 20);    /* Warning */
    int cmp2 = memcmp(signed_memory, plain_memory, 20);       /* Warning */
    int cmp3 = memcmp(unsigned_memory, plain_memory, 20);     /* Warning */

    printf("\nMemory comparison results:\n");
    printf("signed vs unsigned: %d\n", cmp1);
    printf("signed vs plain: %d\n", cmp2);
    printf("unsigned vs plain: %d\n", cmp3);

    /* VIOLATION: memmove with type mismatches */
    signed char overlap_source[] = "Overlapping data test";
    memmove(overlap_source + 5, overlap_source, 10);  /* OK - same type */

    /* Cross-type memmove */
    memmove(unsigned_memory, signed_memory + 5, 15);  /* Warning */

    printf("\nAfter memmove operations:\n");
    printf("Overlap source: %s\n", overlap_source);           /* Warning */
    printf("Unsigned destination: %s\n", unsigned_memory);    /* Warning */

    /* VIOLATION: Memory search operations */
    signed char search_data[] = "Find the X character";
    signed char *found_signed = memchr(search_data, 'X', strlen((char*)search_data));

    unsigned char *found_unsigned = memchr(unsigned_memory, 'X', 50);  /* OK if searching unsigned */

    /* Cross-type search */
    char *found_plain = memchr(signed_memory, 'T', 50);  /* Warning */

    printf("\nMemory search results:\n");
    if (found_signed) {
        printf("Found in signed: position %ld\n", found_signed - search_data);
    }
    if (found_unsigned) {
        printf("Found in unsigned: position %ld\n", found_unsigned - unsigned_memory);
    }
    if (found_plain) {
        printf("Found in cross-search: position %ld\n", found_plain - (char*)signed_memory);
    }

    /* VIOLATION: Memory allocation with wrong size assumptions */
    size_t string_length = 50;

    /* Assuming all character types have same size (usually true, but conceptually wrong) */
    signed char *sized_signed = malloc(string_length);           /* Should be explicit about type */
    unsigned char *sized_unsigned = malloc(string_length);       /* Should be explicit about type */

    /* VIOLATION: Memory copy with size calculation issues */
    strcpy(sized_signed, "Sized allocation test");              /* Warning */
    strcpy(sized_unsigned, "Another sized test");               /* Warning */

    /* VIOLATION: realloc with type changes */
    signed_memory = realloc(signed_memory, 200);
    if (signed_memory == NULL) {
        printf("Realloc failed\n");
        goto cleanup;
    }

    /* Cross-type assignment after realloc */
    unsigned_memory = (unsigned char*)signed_memory;  /* Type conversion */

    /* VIOLATION: Memory pattern operations */
    char pattern[] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00};

    /* Fill with pattern using different types */
    for (size_t i = 0; i < 20; i += 4) {
        signed_memory[i] = pattern[0];     /* Sign issues with 0xAA */
        signed_memory[i+1] = pattern[1];   /* Sign issues with 0xBB */
        signed_memory[i+2] = pattern[2];   /* Sign issues with 0xCC */
        signed_memory[i+3] = pattern[3];   /* Sign issues with 0xDD */
    }

    printf("\nPattern fill results:\n");
    for (size_t i = 0; i < 8; i++) {
        printf("signed_memory[%zu]: %d (0x%02X)\n",
               i, signed_memory[i], (unsigned char)signed_memory[i]);
    }

    /* VIOLATION: Memory zeroing with different interpretations */
    memset(plain_memory, 0, 100);
    memset(signed_memory, 0, 100);
    memset(unsigned_memory, 0, 100);

    /* Check if they're all zero (they should be, but the interpretation differs) */
    int all_zero_plain = 1;
    int all_zero_signed = 1;
    int all_zero_unsigned = 1;

    for (size_t i = 0; i < 10; i++) {
        if (plain_memory[i] != 0) all_zero_plain = 0;
        if (signed_memory[i] != 0) all_zero_signed = 0;
        if (unsigned_memory[i] != 0) all_zero_unsigned = 0;
    }

    printf("\nZero check results:\n");
    printf("Plain all zero: %d\n", all_zero_plain);
    printf("Signed all zero: %d\n", all_zero_signed);
    printf("Unsigned all zero: %d\n", all_zero_unsigned);

cleanup:
    /* Cleanup */
    free(signed_memory);
    free(plain_memory);
    free(sized_signed);
    free(sized_unsigned);
    /* Note: unsigned_memory points to signed_memory, so don't double-free */

    return 0;
}