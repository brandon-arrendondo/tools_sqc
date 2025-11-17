/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: struct_immutable_fields.c
 *
 * This case demonstrates violations where structure fields that
 * are never modified after initialization lack const qualification.
 */

#include <stdio.h>
#include <string.h>

struct Configuration {
    /* NON-COMPLIANT: These fields should be const if never modified */
    char name[50];
    int id;
    double threshold;
    int max_retries;
};

struct Point {
    /* NON-COMPLIANT: Coordinates that don't change should be const */
    double x;
    double y;
    double z;
};

void process_configuration(void) {
    /* NON-COMPLIANT: Config struct with immutable data */
    struct Configuration config = {
        "Production",
        12345,
        0.95,
        3
    };
    
    /* Fields are only read, never modified */
    printf("Configuration:\n");
    printf("  Name: %s\n", config.name);
    printf("  ID: %d\n", config.id);
    printf("  Threshold: %.2f\n", config.threshold);
    printf("  Max retries: %d\n", config.max_retries);
    
    /* Using the values but not modifying them */
    for (int i = 0; i < config.max_retries; i++) {
        if (config.threshold > 0.9) {
            printf("Retry %d for config %s\n", i + 1, config.name);
        }
    }
}

void geometric_calculations(void) {
    /* NON-COMPLIANT: Fixed points should use const */
    struct Point origin = {0.0, 0.0, 0.0};
    struct Point vertex1 = {1.0, 0.0, 0.0};
    struct Point vertex2 = {0.0, 1.0, 0.0};
    struct Point vertex3 = {0.0, 0.0, 1.0};
    
    printf("\nGeometric Points:\n");
    printf("Origin: (%.1f, %.1f, %.1f)\n", origin.x, origin.y, origin.z);
    printf("Vertex1: (%.1f, %.1f, %.1f)\n", vertex1.x, vertex1.y, vertex1.z);
    printf("Vertex2: (%.1f, %.1f, %.1f)\n", vertex2.x, vertex2.y, vertex2.z);
    printf("Vertex3: (%.1f, %.1f, %.1f)\n", vertex3.x, vertex3.y, vertex3.z);
    
    /* Calculate distances - points are never modified */
    double dist1 = vertex1.x * vertex1.x + vertex1.y * vertex1.y + vertex1.z * vertex1.z;
    double dist2 = vertex2.x * vertex2.x + vertex2.y * vertex2.y + vertex2.z * vertex2.z;
    
    printf("Distance from origin: v1=%.2f, v2=%.2f\n", dist1, dist2);
}

int main(void) {
    struct {
        /* NON-COMPLIANT: Anonymous struct with immutable fields */
        char product_name[30];
        int product_code;
        float price;
    } product = {"Widget", 1001, 19.99f};
    
    printf("Product Information:\n");
    printf("  Name: %s\n", product.product_name);
    printf("  Code: %d\n", product.product_code);
    printf("  Price: $%.2f\n", product.price);
    
    process_configuration();
    geometric_calculations();
    
    return 0;
}