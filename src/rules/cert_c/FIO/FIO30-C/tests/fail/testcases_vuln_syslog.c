/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in syslog
 */

#include <stdio.h>
#include <syslog.h>

int main() {
    char log_message[100];

    printf("Enter log message: ");
    fgets(log_message, sizeof(log_message), stdin);

    openlog("vulnerable_app", LOG_PID, LOG_USER);

    // VULNERABLE: user input as syslog format string
    syslog(LOG_INFO, log_message);

    closelog();
    return 0;
}