/*
 * Rule: FIO18-C
 * Source: testcases
 * Status: FAIL - fwrite() count not derived from strlen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fwrite with sizeof - may write past null terminator */
void fwrite_sizeof(void) {
    char buffer[256] = "hello";
    FILE *fp = fopen("out.txt", "w");
    fwrite(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
}

/* fwrite with unrelated variable for count */
void fwrite_unrelated_count(void) {
    char *data = "test data";
    size_t count = 1024;
    FILE *fp = fopen("out.txt", "w");
    fwrite(data, 1, count, fp);
    fclose(fp);
}

/* fwrite with a parameter not derived from strlen */
void fwrite_param_count(char *buf, size_t n) {
    FILE *fp = fopen("out.txt", "w");
    fwrite(buf, 1, n, fp);
    fclose(fp);
}

/* fwrite with hardcoded variable not from strlen */
void fwrite_wrong_size(void) {
    char msg[100];
    size_t sz = 100;
    FILE *fp = fopen("log.txt", "w");
    fwrite(msg, 1, sz, fp);
    fclose(fp);
}
