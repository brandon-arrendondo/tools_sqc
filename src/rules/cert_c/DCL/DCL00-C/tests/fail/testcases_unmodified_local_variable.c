/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: unmodified_local_variable.c
 *
 * This case demonstrates a violation where a local variable is never
 * modified after initialization but is not declared as const.
 */

#include <stdio.h>

void calculate_area(void) {
    /* NON-COMPLIANT: pi is never modified but not const-qualified */
    float pi = 3.14159f;
    float radius = 5.0f;
    
    /* pi is used but never modified */
    float area = pi * radius * radius;
    
    printf("Area of circle: %.2f\n", area);
}

void process_data(void) {
    /* NON-COMPLIANT: max_size is never modified but not const-qualified */
    int max_size = 100;
    int current_size = 0;
    
    while (current_size < max_size) {
        /* max_size is only read, never written */
        printf("Processing item %d of %d\n", current_size, max_size);
        current_size++;
    }
}

int main(void) {
    /* NON-COMPLIANT: version is never modified but not const-qualified */
    char version[] = "1.0.0";
    
    printf("Application version: %s\n", version);
    
    calculate_area();
    process_data();
    
    return 0;
}