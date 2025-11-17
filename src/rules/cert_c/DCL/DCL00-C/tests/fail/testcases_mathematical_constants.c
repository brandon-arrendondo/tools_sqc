/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: mathematical_constants.c
 *
 * This case demonstrates violations where mathematical constants
 * are not const-qualified despite never being modified.
 */

#include <stdio.h>
#include <math.h>

double calculate_circle_properties(double radius) {
    /* NON-COMPLIANT: Mathematical constants should be const */
    double pi = 3.141592653589793;
    double euler = 2.718281828459045;
    
    /* Using constants but never modifying them */
    double circumference = 2 * pi * radius;
    double area = pi * radius * radius;
    double exponential = pow(euler, radius);
    
    printf("Circumference: %.2f\n", circumference);
    printf("Area: %.2f\n", area);
    printf("e^r: %.2f\n", exponential);
    
    return area;
}

double physics_calculations(double mass) {
    /* NON-COMPLIANT: Physics constants should be const */
    double gravity = 9.80665;  /* m/s^2 */
    double speed_of_light = 299792458.0;  /* m/s */
    double planck = 6.62607015e-34;  /* J⋅s */
    
    double weight = mass * gravity;
    double energy = mass * speed_of_light * speed_of_light;
    
    printf("Weight: %.2f N\n", weight);
    printf("Energy (E=mc²): %.2e J\n", energy);
    printf("Planck constant: %.2e J⋅s\n", planck);
    
    return energy;
}

int main(void) {
    /* NON-COMPLIANT: Conversion factors should be const */
    double meters_per_mile = 1609.344;
    double kg_per_pound = 0.45359237;
    
    double distance_miles = 10.0;
    double distance_meters = distance_miles * meters_per_mile;
    
    printf("Distance: %.2f miles = %.2f meters\n", distance_miles, distance_meters);
    
    calculate_circle_properties(5.0);
    physics_calculations(10.0);
    
    return 0;
}