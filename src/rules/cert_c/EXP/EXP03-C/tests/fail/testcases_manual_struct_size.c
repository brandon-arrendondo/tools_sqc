/*
 * Rule: EXP03-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP03-C violation
 * Description: Manual struct size calculation with sizeof additions
 */

#include <stdlib.h>
#include <string.h>

struct record {
    int id;
    double value;
    char name[32];
};

void copy_record(const struct record *src) {
    struct record *dst = (struct record *)malloc(
        sizeof(int) + sizeof(double) + 32 * sizeof(char)
    );  /* Violation: manual sum ignores padding */

    if (dst == NULL) return;
    memcpy(dst, src, sizeof(struct record));
    free(dst);
}
