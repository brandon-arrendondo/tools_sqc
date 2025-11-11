/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: logging_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

/* NON-COMPLIANT: Log message modification */
void unsafe_log_message_modification(void) {
    errno = EACCES;
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Adding log prefix */
        memmove(error_msg + 8, error_msg, strlen(error_msg) + 1);
        memcpy(error_msg, "[ERROR] ", 8);  /* Undefined behavior */
        printf("Prefixed error: %s\n", error_msg);
    }
}

/* NON-COMPLIANT: Timestamp formatting */
void unsafe_timestamp_formatting(void) {
    time_t now = time(NULL);
    char *time_str = ctime(&now);

    if (time_str != NULL && strlen(time_str) > 0) {
        /* VIOLATION: Removing newline for logging */
        time_str[strlen(time_str) - 1] = '\0';  /* Undefined behavior */
        printf("Log timestamp: %s\n", time_str);
    }
}

int main(void) {
    unsafe_log_message_modification();
    unsafe_timestamp_formatting();
    return 0;
}