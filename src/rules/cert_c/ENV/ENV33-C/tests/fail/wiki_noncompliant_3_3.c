/*
 * Rule: ENV33-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV33-C violation
 *
 * Demonstrates command injection through environment variable
 */

#include <stdlib.h>

void execute_command(void) {
    char *cmd_str = getenv("CMD");

    /* VIOLATION: Direct use of getenv() in system() */
    if (cmd_str) {
        system(cmd_str);  /* Attacker can inject: "happy'; useradd 'attacker" */
    }
}

int main(void) {
    execute_command();
    return 0;
}
