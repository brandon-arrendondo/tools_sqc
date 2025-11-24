/*
 * Rule: STR01-C
 * Source: manual
 * Status: FAIL - Should trigger STR01-C violation
 * 
 * Mixed string management: static array and dynamic allocation
 */

#include <stdlib.h>
#include <string.h>

void mixed_string_handling() {
    // Static string management
    char buffer[100] = "static string";
    
    // Dynamic string management
    char *dynamic = malloc(100);
    strcpy(dynamic, "dynamic string");
    
    free(dynamic);
}
