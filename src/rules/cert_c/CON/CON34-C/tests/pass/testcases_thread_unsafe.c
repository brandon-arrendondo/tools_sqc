/*
 * Rule: CON34-C
 * Source: testcases
 * Status: PASS - Known limitation: pattern not detected
 * TODO: Move to fail/ when implemented (see PLAN.md)
 */

#include <time.h>
#include <stdlib.h>

/* localtime is not thread-safe */
void use_localtime(void) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    (void)tm;
}

/* strtok is not thread-safe */
void use_strtok(char *str) {
    char *tok = strtok(str, ",");
    (void)tok;
}

/* rand is not thread-safe */
int get_random(void) {
    return rand();
}
