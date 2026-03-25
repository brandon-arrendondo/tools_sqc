/*
 * Rule: ENV03-C
 * Source: testcases
 * Status: FAIL - Environment variable used without sanitization
 */

#include <stdlib.h>
#include <stdio.h>

/* Direct use of getenv in system() */
void unsafe_env_exec(void) {
    char *cmd = getenv("CMD");
    system(cmd);
}

/* getenv used in printf format */
void unsafe_env_print(void) {
    char *fmt = getenv("FMT");
    printf(fmt);
}
