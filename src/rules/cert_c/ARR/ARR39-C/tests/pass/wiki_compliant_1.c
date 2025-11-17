/*
 * Rule: ARR39-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

enum { INTBUFSIZE = 80 };

extern int getdata(void);
int buf[INTBUFSIZE];

void func(void) {
  int *buf_ptr = buf;

  while (buf_ptr < (buf + INTBUFSIZE)) {
    *buf_ptr++ = getdata();
  }
}