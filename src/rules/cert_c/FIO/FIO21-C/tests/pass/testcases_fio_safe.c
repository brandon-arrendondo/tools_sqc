/*
 * Rule: FIO21-C
 * Source: testcases
 * Status: PASS - No insecure temporary file operations
 */

#include <stdio.h>

/* Using mkstemp — secure temp file */
void use_mkstemp(void) {
    char template[] = "/tmp/myapp-XXXXXX";
    int fd = mkstemp(template);
    if (fd >= 0) {
        close(fd);
    }
}

/* No temp file operations */
void no_temp_files(void) {
    FILE *f = fopen("data.txt", "r");
    if (f) fclose(f);
}
