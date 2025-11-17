/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Adding sizeof(buf) to int pointer causes double-scaling
 */

enum { INTBUFSIZE = 80 };

extern int getdata(void);
int buf[INTBUFSIZE];

void func(void) {
    int *buf_ptr = buf;

    // sizeof(buf) returns bytes, which gets scaled again as int*
    while (buf_ptr < (buf + sizeof(buf))) {  // Line 15 - VIOLATION
        *buf_ptr++ = getdata();
    }
}

int main(void) {
    func();
    return 0;
}
