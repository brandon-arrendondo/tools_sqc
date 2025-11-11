/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: array_not_const.c
 *
 * This case demonstrates violations where arrays that are never
 * modified after initialization are not const-qualified.
 */

#include <stdio.h>

void display_lookup_tables(void) {
    /* NON-COMPLIANT: Lookup table should be const */
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    /* NON-COMPLIANT: Hex digit lookup table should be const */
    char hex_digits[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    
    /* NON-COMPLIANT: Prime numbers array should be const */
    int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    
    printf("Days in each month:\n");
    for (int i = 0; i < 12; i++) {
        printf("Month %2d: %d days\n", i + 1, days_in_month[i]);
    }
    
    printf("\nHex digits:\n");
    for (int i = 0; i < 16; i++) {
        printf("%d = %c\n", i, hex_digits[i]);
    }
    
    printf("\nFirst 10 primes:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", primes[i]);
    }
    printf("\n");
}

void color_palette(void) {
    /* NON-COMPLIANT: RGB color values should be const */
    unsigned char red[] = {255, 0, 0};
    unsigned char green[] = {0, 255, 0};
    unsigned char blue[] = {0, 0, 255};
    unsigned char white[] = {255, 255, 255};
    unsigned char black[] = {0, 0, 0};
    
    printf("\nColor Palette (RGB):\n");
    printf("Red: (%d, %d, %d)\n", red[0], red[1], red[2]);
    printf("Green: (%d, %d, %d)\n", green[0], green[1], green[2]);
    printf("Blue: (%d, %d, %d)\n", blue[0], blue[1], blue[2]);
    printf("White: (%d, %d, %d)\n", white[0], white[1], white[2]);
    printf("Black: (%d, %d, %d)\n", black[0], black[1], black[2]);
}

int main(void) {
    /* NON-COMPLIANT: Command names array should be const */
    char commands[][10] = {"help", "quit", "save", "load", "print"};
    
    printf("Available commands:\n");
    for (int i = 0; i < 5; i++) {
        printf("  - %s\n", commands[i]);
    }
    
    display_lookup_tables();
    color_palette();
    
    return 0;
}