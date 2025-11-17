/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses snprintf with literal format string, bounds checking included
 */

#include <stdio.h>

int main() {
    char buffer[100];
    char product[50];
    double price;

    printf("Enter product name: ");
    scanf("%49s", product);
    printf("Enter price: ");
    scanf("%lf", &price);

    // Safe: literal format string with size limit
    snprintf(buffer, sizeof(buffer), "Product: %s - Price: $%.2f", product, price);
    printf("Receipt: %s\n", buffer);

    return 0;
}