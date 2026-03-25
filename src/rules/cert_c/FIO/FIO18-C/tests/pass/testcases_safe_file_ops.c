/*
 * Rule: FIO18-C
 * Source: testcases
 * Status: PASS - fwrite() count properly derived from strlen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fwrite with strlen inline */
void fwrite_strlen_inline(void) {
    char buffer[] = "hello world";
    FILE *fp = fopen("out.txt", "w");
    fwrite(buffer, 1, strlen(buffer) + 1, fp);
    fclose(fp);
}

/* fwrite with variable assigned from strlen */
void fwrite_strlen_var(void) {
    char *data = "test data";
    size_t len = strlen(data) + 1;
    FILE *fp = fopen("out.txt", "w");
    fwrite(data, 1, len, fp);
    fclose(fp);
}

/* fwrite with numeric literal count (not a variable) */
void fwrite_literal_count(void) {
    char msg[] = "OK";
    FILE *fp = fopen("out.txt", "w");
    fwrite(msg, 1, 3, fp);
    fclose(fp);
}

/* fread is not checked by this rule */
void fread_usage(void) {
    char buffer[256];
    FILE *fp = fopen("in.txt", "r");
    fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
}

/* No fwrite call at all */
void no_fwrite(void) {
    FILE *fp = fopen("out.txt", "w");
    fprintf(fp, "hello\n");
    fclose(fp);
}
