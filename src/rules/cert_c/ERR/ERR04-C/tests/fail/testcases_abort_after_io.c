/*
 * Rule: ERR04-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR04-C violation
 * Description: abort() called in function that does file I/O
 */

#include <stdio.h>
#include <stdlib.h>

void log_and_die(const char *msg) {
    FILE *log = fopen("error.log", "a");
    if (log != NULL) {
        fprintf(log, "FATAL: %s\n", msg);
    }
    abort();  /* Violation: buffered data may be lost */
}
