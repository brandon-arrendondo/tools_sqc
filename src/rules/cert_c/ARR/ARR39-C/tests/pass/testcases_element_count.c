/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using element count instead of sizeof for pointer arithmetic
 */

enum { INTBUFSIZE = 80 };

extern int getdata(void);
int buf[INTBUFSIZE];

void func(void) {
    int *buf_ptr = buf;

    // Use element count, not sizeof - COMPLIANT
    while (buf_ptr < (buf + INTBUFSIZE)) {
        *buf_ptr++ = getdata();
    }
}

int main(void) {
    func();
    return 0;
}
