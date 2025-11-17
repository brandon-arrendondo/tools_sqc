/*
 * Rule: EXP08-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP08-C violation
 */

int buf[INTBUFSIZE];
int *buf_ptr = buf;

while (havedata() && buf_ptr < (buf + sizeof(buf))) {
  *buf_ptr++ = parseint(getdata());
}