/*
 * Rule: EXP03-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP03-C violation
 * Description: Proper sizeof(struct) usage for allocation
 */

#include <stdlib.h>
#include <string.h>

struct record {
    int id;
    double value;
    char name[32];
};

void copy_record(const struct record *src) {
    struct record *dst = (struct record *)malloc(sizeof(struct record));
    if (dst == NULL) return;
    memcpy(dst, src, sizeof(struct record));
    free(dst);
}

void alloc_array(int count) {
    struct record *arr = calloc(count, sizeof(struct record));
    if (arr == NULL) return;
    free(arr);
}
