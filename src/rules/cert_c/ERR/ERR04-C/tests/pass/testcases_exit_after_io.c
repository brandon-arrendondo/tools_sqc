/*
 * Rule: ERR04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR04-C violation
 * Description: exit() properly flushes buffers after file I/O
 */

#include <stdio.h>
#include <stdlib.h>

void log_and_exit(const char *msg) {
    FILE *log = fopen("error.log", "a");
    if (log != NULL) {
        fprintf(log, "FATAL: %s\n", msg);
        fclose(log);
    }
    exit(EXIT_FAILURE);  /* Safe: exit flushes stdio */
}
