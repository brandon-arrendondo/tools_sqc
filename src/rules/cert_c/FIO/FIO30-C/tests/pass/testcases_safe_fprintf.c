/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses literal format string with fprintf, user data passed as argument
 */

#include <stdio.h>

int main() {
    FILE *log_file = fopen("log.txt", "w");
    char username[50];
    int user_id;

    printf("Enter username: ");
    scanf("%49s", username);
    printf("Enter user ID: ");
    scanf("%d", &user_id);

    if (log_file) {
        // Safe: literal format string with proper specifiers
        fprintf(log_file, "User login: %s (ID: %d)\n", username, user_id);
        fclose(log_file);
    }

    return 0;
}