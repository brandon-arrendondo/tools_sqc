/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Logging function uses predefined format, user data as arguments
 */

#include <stdio.h>
#include <time.h>

void log_event(const char *event_type, const char *user_data) {
    time_t now = time(NULL);
    char *timestamp = ctime(&now);

    // Safe: literal format string, user data as argument
    fprintf(stderr, "[%s] Event: %s - Data: %s", timestamp, event_type, user_data);
}

int main() {
    char user_action[100];

    printf("Enter action performed: ");
    fgets(user_action, sizeof(user_action), stdin);

    // Safe: user input passed as data, not format string
    log_event("USER_ACTION", user_action);

    return 0;
}