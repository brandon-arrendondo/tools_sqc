/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses literal format string with scanf for input parsing
 */

#include <stdio.h>

int main() {
    char name[50];
    int age;
    float height;

    printf("Enter personal information:\n");

    // Safe: literal format strings for input parsing
    printf("Name: ");
    scanf("%49s", name);

    printf("Age: ");
    scanf("%d", &age);

    printf("Height (in meters): ");
    scanf("%f", &height);

    printf("\nInformation entered:\n");
    printf("Name: %s\n", name);
    printf("Age: %d\n", age);
    printf("Height: %.2f meters\n", height);

    return 0;
}