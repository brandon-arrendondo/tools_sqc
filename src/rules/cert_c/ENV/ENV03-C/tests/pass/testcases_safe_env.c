/*
 * Rule: ENV03-C
 * Source: testcases
 * Status: PASS - Environment variable properly validated
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* getenv with null check before use */
void safe_env_use(void) {
    const char *val = getenv("HOME");
    if (val != NULL && strlen(val) < 256) {
        printf("Home: %s\n", val);
    }
}
