/*
 * Rule: STR01-C
 * Source: manual
 * Status: PASS - Should NOT trigger violation
 *
 * Consistent string management: only static arrays
 */

#include <string.h>

void static_only_string_handling() {
    // Only static string management
    char buffer1[100] = "string one";
    char buffer2[50] = "string two";

    strncpy(buffer1, buffer2, sizeof(buffer1));
}
