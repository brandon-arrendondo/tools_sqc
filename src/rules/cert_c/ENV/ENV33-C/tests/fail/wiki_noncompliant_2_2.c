/*
 * Rule: ENV33-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV33-C violation
 *
 * Demonstrates passing unsanitized environment variable to system()
 * which can lead to command injection
 */

#include <stdlib.h>
#include <string.h>

void process_request(void) {
    char *user_input = getenv("USER_CMD");
    char cmd[256];

    /* VIOLATION: Directly using getenv() result in system() call */
    if (user_input != NULL) {
        snprintf(cmd, sizeof(cmd), "process %s", user_input);
        system(cmd);  /* Command injection vulnerability */
    }
}

int main(void) {
    process_request();
    return 0;
}
