/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses literal format string with sprintf, user input as data argument
 */

#include <stdio.h>
#include <string.h>

int main() {
    char message[200];
    char user_name[50];
    int age;

    printf("Enter your name: ");
    fgets(user_name, sizeof(user_name), stdin);
    // Remove newline
    user_name[strcspn(user_name, "\n")] = 0;

    printf("Enter your age: ");
    scanf("%d", &age);

    // Safe: literal format string, user data as arguments
    sprintf(message, "Name: %s, Age: %d years old", user_name, age);
    printf("%s\n", message);

    return 0;
}