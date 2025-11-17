/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: character_arithmetic_operations.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for arithmetic operations, leading to sign-dependent
 * behavior and unexpected results.
 */

#include <stdio.h>

int main(void) {
    /* VIOLATION: Arithmetic operations with signed char */
    signed char a = 100;
    signed char b = 150;  /* May be negative on signed char systems */

    printf("Arithmetic with signed char:\n");
    printf("a = %d, b = %d\n", a, b);

    /* VIOLATION: Addition with potential overflow */
    signed char sum = a + b;  /* Overflow and sign issues */
    printf("a + b = %d (expected ~250, but may overflow)\n", sum);

    /* VIOLATION: Subtraction with sign issues */
    signed char diff = a - b;
    printf("a - b = %d\n", diff);

    /* VIOLATION: Multiplication with severe overflow risk */
    signed char product = a * 2;  /* Likely overflow */
    printf("a * 2 = %d (overflow likely)\n", product);

    /* VIOLATION: Division with sign-dependent behavior */
    if (b != 0) {
        signed char quotient = a / b;  /* Sign-dependent result */
        printf("a / b = %d\n", quotient);
    }

    /* VIOLATION: Character range arithmetic */
    printf("\nCharacter range arithmetic:\n");

    signed char char_start = 'A';
    signed char char_end = 'Z';
    signed char range = char_end - char_start;  /* Should be 25 */

    printf("'Z' - 'A' = %d\n", range);

    /* VIOLATION: Wraparound in character arithmetic */
    signed char wrap_test = 120;
    printf("Starting value: %d\n", wrap_test);

    for (int i = 0; i < 10; i++) {
        wrap_test += 20;  /* Will wrap around */
        printf("After adding 20 (iteration %d): %d\n", i + 1, wrap_test);
    }

    /* VIOLATION: Bitwise operations with character types */
    printf("\nBitwise operations:\n");

    signed char bits1 = 0x55;  /* 01010101 */
    signed char bits2 = 0xAA;  /* 10101010 - may be negative */

    printf("bits1 = 0x%02X (%d)\n", (unsigned char)bits1, bits1);
    printf("bits2 = 0x%02X (%d)\n", (unsigned char)bits2, bits2);

    /* VIOLATION: Bitwise AND with sign extension issues */
    signed char and_result = bits1 & bits2;
    printf("bits1 & bits2 = 0x%02X (%d)\n", (unsigned char)and_result, and_result);

    /* VIOLATION: Bitwise OR with sign issues */
    signed char or_result = bits1 | bits2;
    printf("bits1 | bits2 = 0x%02X (%d)\n", (unsigned char)or_result, or_result);

    /* VIOLATION: XOR operations */
    signed char xor_result = bits1 ^ bits2;
    printf("bits1 ^ bits2 = 0x%02X (%d)\n", (unsigned char)xor_result, xor_result);

    /* VIOLATION: Bit shifting with character types */
    printf("\nBit shifting operations:\n");

    signed char shift_value = 0x01;
    for (int shift = 0; shift < 8; shift++) {
        signed char left_shifted = shift_value << shift;  /* Sign issues */
        printf("0x01 << %d = 0x%02X (%d)\n",
               shift, (unsigned char)left_shifted, left_shifted);
    }

    /* VIOLATION: Right shift with sign extension */
    signed char negative_value = -1;  /* 0xFF */
    printf("\nRight shift of negative value:\n");
    printf("Starting value: %d (0x%02X)\n", negative_value, (unsigned char)negative_value);

    for (int shift = 1; shift <= 4; shift++) {
        signed char right_shifted = negative_value >> shift;  /* Implementation-defined */
        printf(">> %d = %d (0x%02X)\n",
               shift, right_shifted, (unsigned char)right_shifted);
    }

    /* VIOLATION: Modulo operations with character types */
    printf("\nModulo operations:\n");

    signed char dividend = 100;
    signed char divisor = 7;
    signed char remainder = dividend % divisor;

    printf("%d %% %d = %d\n", dividend, divisor, remainder);

    /* VIOLATION: Increment/decrement with wraparound */
    printf("\nIncrement/decrement wraparound:\n");

    signed char max_char = 127;  /* Maximum for signed char */
    printf("max_char = %d\n", max_char);

    max_char++;  /* Wraparound to -128 */
    printf("After increment: %d\n", max_char);

    signed char min_char = -128;  /* Minimum for signed char */
    printf("min_char = %d\n", min_char);

    min_char--;  /* Wraparound to 127 */
    printf("After decrement: %d\n", min_char);

    /* VIOLATION: Comparison operations with sign issues */
    printf("\nComparison operations:\n");

    signed char positive = 50;
    signed char negative = -50;
    unsigned char high_unsigned = 200;

    printf("positive = %d, negative = %d, high_unsigned = %d\n",
           positive, negative, high_unsigned);

    /* VIOLATION: Comparing signed and unsigned characters */
    if (positive > (signed char)high_unsigned) {  /* Sign conversion */
        printf("positive > high_unsigned (as signed)\n");
    } else {
        printf("positive <= high_unsigned (as signed)\n");
    }

    /* VIOLATION: Magnitude calculations */
    printf("\nMagnitude calculations:\n");

    signed char values[] = {-100, -50, 0, 50, 100};
    for (size_t i = 0; i < 5; i++) {
        signed char val = values[i];
        signed char abs_val = (val < 0) ? -val : val;  /* May overflow for -128 */

        printf("abs(%d) = %d\n", val, abs_val);
    }

    /* VIOLATION: Hash calculation with character arithmetic */
    printf("\nHash calculation:\n");

    signed char hash_input[] = "Hash input string";
    signed char hash = 0;

    for (size_t i = 0; hash_input[i] != '\0'; i++) {
        /* VIOLATION: Simple hash with overflow and sign issues */
        hash = hash * 31 + hash_input[i];  /* Multiple overflows */
    }

    printf("Simple hash result: %d (0x%02X)\n", hash, (unsigned char)hash);

    /* VIOLATION: Checksum calculation */
    printf("\nChecksum calculation:\n");

    unsigned char data[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    signed char checksum = 0;  /* Should use unsigned for checksums */

    for (size_t i = 0; i < sizeof(data); i++) {
        checksum += (signed char)data[i];  /* Sign conversion and overflow */
    }

    printf("Checksum result: %d (0x%02X)\n", checksum, (unsigned char)checksum);

    /* VIOLATION: Base conversion arithmetic */
    printf("\nBase conversion:\n");

    signed char base10_digit = '7';
    signed char base16_digit = 'F';

    /* Convert to numeric values */
    signed char numeric_10 = base10_digit - '0';  /* Should be 7 */
    signed char numeric_16 = base16_digit - 'A' + 10;  /* Should be 15 */

    printf("'%c' as number: %d\n", base10_digit, numeric_10);
    printf("'%c' as hex: %d\n", base16_digit, numeric_16);

    return 0;
}