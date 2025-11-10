int buf[INTBUFSIZE];
int *buf_ptr = buf;

while (havedata() && buf_ptr < &buf[INTBUFSIZE]) {
  *buf_ptr++ = parseint(getdata());
}