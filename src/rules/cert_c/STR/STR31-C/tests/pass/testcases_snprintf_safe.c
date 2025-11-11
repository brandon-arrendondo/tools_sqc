/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Uses snprintf to prevent buffer overflow by limiting output size
 */

#include <stdio.h>

int main() {
    char buffer[20];
    char name[] = "John";
    int age = 25;

    snprintf(buffer, sizeof(buffer), "Name: %s, Age: %d", name, age);
    printf("Formatted safely: %s\n", buffer);

    return 0;
}