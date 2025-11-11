/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: global_constants.c
 *
 * This case demonstrates violations where global variables that
 * act as constants are not const-qualified.
 */

#include <stdio.h>

/* NON-COMPLIANT: Global constants should be const-qualified */
int MAX_BUFFER_SIZE = 1024;
float TAX_RATE = 0.08f;
char APPLICATION_NAME[] = "MyApplication";
double EPSILON = 1e-9;

/* NON-COMPLIANT: Global lookup table should be const */
int ERROR_CODES[] = {0, -1, -2, -3, -4, -5};
char *ERROR_MESSAGES[] = {
    "Success",
    "General Error",
    "File Not Found",
    "Access Denied",
    "Invalid Parameter",
    "Out of Memory"
};

void display_application_info(void) {
    /* Using global constants - they are never modified */
    printf("Application: %s\n", APPLICATION_NAME);
    printf("Max buffer size: %d bytes\n", MAX_BUFFER_SIZE);
    printf("Epsilon value: %e\n", EPSILON);
}

void calculate_price(double base_price) {
    /* Using global TAX_RATE - never modified */
    double tax_amount = base_price * TAX_RATE;
    double total = base_price + tax_amount;
    
    printf("Base price: $%.2f\n", base_price);
    printf("Tax (%.1f%%): $%.2f\n", TAX_RATE * 100, tax_amount);
    printf("Total: $%.2f\n", total);
}

void show_error_codes(void) {
    printf("\nError Code Reference:\n");
    
    /* Using global error tables - never modified */
    for (int i = 0; i < 6; i++) {
        printf("  Code %2d: %s\n", ERROR_CODES[i], ERROR_MESSAGES[i]);
    }
}

int main(void) {
    /* NON-COMPLIANT: File-scope constants within main */
    static int VERSION_MAJOR = 1;
    static int VERSION_MINOR = 0;
    static int VERSION_PATCH = 0;
    
    printf("Version: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("\n");
    
    display_application_info();
    printf("\n");
    
    calculate_price(100.00);
    
    show_error_codes();
    
    return 0;
}