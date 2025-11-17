/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: Hardcoded struct size ignoring padding
 */

#include <stdint.h>
#include <stdio.h>

struct obj {
    char c;
    long long i;
};

void hardcoded_struct_size(FILE *f, struct obj *objs, size_t num_objs) {
    // Assumes 16 bytes but padding may differ
    const size_t obj_size = 16;  // Line 17 - VIOLATION

    if (num_objs > (SIZE_MAX / obj_size) ||
        num_objs != fwrite(objs, obj_size, num_objs, f)) {
        // Handle error
    }
}

int main(void) {
    struct obj objs[5] = {{0}};
    FILE *f = fopen("test.dat", "wb");
    if (f) {
        hardcoded_struct_size(f, objs, 5);
        fclose(f);
    }
    return 0;
}
