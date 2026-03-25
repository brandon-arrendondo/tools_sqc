/*
 * Rule: FIO21-C
 * Source: testcases
 * Status: FAIL - Creating temporary files in shared directories
 */

#include <stdio.h>
#include <stdlib.h>

/* Using tmpnam — insecure temp file creation */
void use_tmpnam(void) {
    char *name = tmpnam(NULL);
    FILE *f = fopen(name, "w");
    if (f) fclose(f);
}

/* Using tempnam — insecure */
void use_tempnam(void) {
    char *name = tempnam("/tmp", "pre");
    FILE *f = fopen(name, "w");
    if (f) fclose(f);
    free(name);
}
