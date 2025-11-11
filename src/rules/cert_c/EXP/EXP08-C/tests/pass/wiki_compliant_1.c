/*
 * Rule: EXP08-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP08-C violation
 */

int buf[INTBUFSIZE];
int *buf_ptr = buf;

while (havedata() && buf_ptr < (buf + INTBUFSIZE)) {
  *buf_ptr++ = parseint(getdata());
}