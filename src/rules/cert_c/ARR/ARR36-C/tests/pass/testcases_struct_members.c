/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Comparing pointers to different members of same struct (exception ARR36-C-EX1)
 */

#include <stdio.h>

struct data {
    int member1;
    int member2;
    int member3;
};

void struct_member_compare(void) {
    struct data d = {10, 20, 30};
    int *ptr1 = &d.member1;
    int *ptr2 = &d.member3;

    // Exception: comparing struct members is allowed - COMPLIANT
    if (ptr1 < ptr2) {
        printf("member1 is before member3 in struct\n");
    }
}

int main(void) {
    struct_member_compare();
    return 0;
}
