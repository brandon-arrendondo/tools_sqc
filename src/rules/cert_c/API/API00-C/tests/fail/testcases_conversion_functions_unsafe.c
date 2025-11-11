/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: conversion_functions_unsafe.c
 *
 * This case demonstrates violations where conversion functions
 * don't validate their parameters for safe conversion.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* NON-COMPLIANT: No validation of string for conversion */
int string_to_integer(const char *str) {
    /* Direct conversion without validation */
    return atoi(str);  /* str could be NULL or contain non-numeric data */
}

/* NON-COMPLIANT: No validation of string for float conversion */
double string_to_double(const char *str) {
    /* Direct conversion without validation */
    return atof(str);  /* str could be NULL or invalid */
}

/* NON-COMPLIANT: No validation of base parameter */
long string_to_long_base(const char *str, int base) {
    /* No validation of base value */
    return strtol(str, NULL, base);  /* base must be 0 or 2-36 */
}

/* NON-COMPLIANT: No validation of buffer for integer to string */
void integer_to_string(int value, char *buffer) {
    /* No validation of buffer */
    sprintf(buffer, "%d", value);  /* buffer could be NULL or too small */
}

/* NON-COMPLIANT: No validation of hex string */
unsigned int hex_string_to_uint(const char *hex_str) {
    unsigned int value;
    /* No validation of hex_str */
    sscanf(hex_str, "%x", &value);  /* hex_str could be NULL or invalid */
    return value;
}

/* NON-COMPLIANT: No validation of character for digit conversion */
int char_to_digit(char c) {
    /* No validation if character is a digit */
    return c - '0';  /* c might not be a digit character */
}

/* NON-COMPLIANT: No validation of ASCII value */
char ascii_to_char(int ascii_value) {
    /* No validation of ASCII range */
    return (char)ascii_value;  /* ascii_value could be out of valid range */
}

/* NON-COMPLIANT: No validation of float to int conversion */
int float_to_int(float value) {
    /* No check for overflow or special values */
    return (int)value;  /* Could overflow if value > INT_MAX */
}

/* NON-COMPLIANT: No validation of pointer to integer conversion */
int pointer_to_int(void *ptr) {
    /* Dangerous conversion without validation */
    return (int)(intptr_t)ptr;  /* Truncation on 64-bit systems */
}

/* NON-COMPLIANT: No validation of binary string */
int binary_string_to_int(const char *binary_str) {
    int result = 0;
    /* No validation of binary_str */
    while (*binary_str) {
        result = result * 2 + (*binary_str - '0');  /* No check if valid binary */
        binary_str++;
    }
    return result;
}

int main(void) {
    char *null_string = NULL;
    char small_buffer[5];

    /* Examples of dangerous conversions */
    // string_to_integer(null_string);  /* NULL string */
    // string_to_integer("not a number");  /* Invalid input */
    // string_to_double(null_string);  /* NULL string */
    // string_to_long_base("123", 40);  /* Invalid base */
    // integer_to_string(1234567890, small_buffer);  /* Buffer overflow */
    // hex_string_to_uint(null_string);  /* NULL string */
    // char_to_digit('X');  /* Not a digit */
    // ascii_to_char(300);  /* Out of ASCII range */
    // float_to_int(1e10f);  /* Overflow */
    // binary_string_to_int("10201");  /* Invalid binary string */

    printf("Conversion functions compiled but lack parameter validation\n");
    return 0;
}