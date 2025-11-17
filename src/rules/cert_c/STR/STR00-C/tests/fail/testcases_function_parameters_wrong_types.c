/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: function_parameters_wrong_types.c
 *
 * This case demonstrates a violation of STR00-C by defining and using
 * functions with inappropriate character parameter types, leading to
 * type compatibility issues and inconsistent behavior.
 */

#include <stdio.h>
#include <string.h>

/* VIOLATION: Function expecting signed char parameter */
void process_signed_string(signed char *str) {
    printf("Processing signed char string: %s\n", str);  /* Warning */

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] > 0) {  /* Sign-dependent behavior */
            printf("  Positive character: %c (%d)\n", str[i], str[i]);
        } else {
            printf("  Non-positive character: (%d)\n", str[i]);
        }
    }
}

/* VIOLATION: Function expecting unsigned char parameter */
void process_unsigned_string(unsigned char *str) {
    printf("Processing unsigned char string: %s\n", str);  /* Warning */

    for (size_t i = 0; str[i] != '\0'; i++) {
        printf("  Character: %c (%u)\n", str[i], str[i]);
    }
}

/* VIOLATION: Function with mixed character parameter types */
int compare_different_types(signed char *s1, unsigned char *s2) {
    /* This function signature creates type compatibility issues */
    printf("Comparing different character types\n");

    /* VIOLATION: Direct comparison between different types */
    while (*s1 && *s2 && (*s1 == *s2)) {  /* Warning: comparison of different types */
        s1++;
        s2++;
    }

    /* VIOLATION: Returning difference with sign issues */
    return *s1 - *s2;  /* Sign extension problems */
}

/* VIOLATION: Function returning wrong character pointer type */
signed char *find_character_signed(signed char *str, signed char target) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == target) {
            return &str[i];
        }
    }
    return NULL;
}

/* VIOLATION: Function parameter type doesn't match intended use */
void print_hex_bytes(char *data, size_t length) {
    /* This function should use unsigned char for byte operations */
    printf("Hex dump using char:\n");
    for (size_t i = 0; i < length; i++) {
        /* VIOLATION: char may be signed, causing display issues */
        printf("%02X ", (unsigned char)data[i]);  /* Cast needed due to wrong parameter type */
    }
    printf("\n");
}

/* VIOLATION: Function with character type assumptions */
int count_uppercase(char *str) {
    int count = 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        /* VIOLATION: Assumes char behavior for character classification */
        if (str[i] >= 'A' && str[i] <= 'Z') {
            count++;
        }
    }
    return count;
}

int main(void) {
    /* Prepare test strings with different character types */
    char plain_string[] = "Plain char string";
    signed char signed_string[] = "Signed char string";
    unsigned char unsigned_string[] = "Unsigned char string";

    printf("Function parameter type mismatches:\n\n");

    /* VIOLATION: Passing different character types to functions */
    process_signed_string(plain_string);      /* Warning */
    process_signed_string(unsigned_string);   /* Warning */

    process_unsigned_string(plain_string);    /* Warning */
    process_unsigned_string(signed_string);   /* Warning */

    /* VIOLATION: Function calls with mixed types */
    int cmp_result = compare_different_types(signed_string, unsigned_string);
    printf("Comparison result: %d\n", cmp_result);

    /* VIOLATION: More cross-type function calls */
    signed char *found = find_character_signed(plain_string, 'P');  /* Warning */
    if (found) {
        printf("Found character at position: %ld\n", found - (signed char*)plain_string);
    }

    /* VIOLATION: Byte operations with wrong parameter type */
    char byte_data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x80, 0x90, 0xFF, 0x00};
    print_hex_bytes(byte_data, 9);

    /* VIOLATION: Character counting with type issues */
    int uppercase_count = count_uppercase(signed_string);  /* Warning */
    printf("Uppercase count: %d\n", uppercase_count);

    /* VIOLATION: Function pointer with character type mismatches */
    void (*string_processor)(char *) = (void (*)(char *))process_signed_string;  /* Warning */
    string_processor(plain_string);

    /* VIOLATION: Library function calls with wrong types */
    size_t len1 = strlen(signed_string);    /* Warning */
    size_t len2 = strlen(unsigned_string);  /* Warning */

    printf("String lengths: %zu, %zu\n", len1, len2);

    /* VIOLATION: String manipulation function calls */
    char dest[100];
    strcpy(dest, signed_string);     /* Warning */
    strcat(dest, " + ");
    strcat(dest, unsigned_string);   /* Warning */

    printf("Concatenated result: %s\n", dest);

    /* VIOLATION: Character search with type mismatches */
    char *pos1 = strchr(signed_string, 'S');    /* Warning */
    char *pos2 = strstr(unsigned_string, "char");  /* Warning */

    if (pos1) printf("Found 'S' in signed string\n");
    if (pos2) printf("Found 'char' in unsigned string\n");

    /* VIOLATION: Memory operations with character type mismatches */
    char buffer[50];
    memcpy(buffer, signed_string, strlen((char*)signed_string));    /* Warning */
    buffer[strlen((char*)signed_string)] = '\0';

    printf("Copied to buffer: %s\n", buffer);

    /* VIOLATION: Variadic function calls with character type issues */
    printf("Printf with character types:\n");
    printf("Signed string: %s\n", signed_string);      /* Warning */
    printf("Unsigned string: %s\n", unsigned_string);  /* Warning */

    /* VIOLATION: Function composition with type mismatches */
    signed char *upper_result = (signed char*)strchr(signed_string, 'S');  /* Warning and cast */
    if (upper_result) {
        printf("Found via composition: %c\n", *upper_result);
    }

    /* VIOLATION: Callback functions with wrong character types */
    int (*comparator)(const void *, const void *) = (int (*)(const void *, const void *))strcmp;

    /* Using with different character types */
    signed char *array1[] = {"first", "second"};
    unsigned char *array2[] = {"alpha", "beta"};

    /* These comparisons involve type mismatches */
    int callback_result1 = comparator(array1[0], array1[1]);  /* Warning */
    int callback_result2 = comparator(array2[0], array2[1]);  /* Warning */

    printf("Callback results: %d, %d\n", callback_result1, callback_result2);

    return 0;
}