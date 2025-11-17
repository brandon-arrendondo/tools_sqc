/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: Using sizeof operator for struct size with padding
 */

#include <stdint.h>
#include <stdio.h>

struct obj {
    char c;
    long long i;
};

void proper_struct_size(FILE *f, struct obj *objs, size_t num_objs) {
    // Use sizeof for actual size including padding - COMPLIANT
    const size_t obj_size = sizeof(*objs);

    if (num_objs > (SIZE_MAX / obj_size) ||
        num_objs != fwrite(objs, obj_size, num_objs, f)) {
        // Handle error
    }
}

int main(void) {
    struct obj objs[5] = {{0}};
    FILE *f = fopen("test.dat", "wb");
    if (f) {
        proper_struct_size(f, objs, 5);
        fclose(f);
    }
    return 0;
}
