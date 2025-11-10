int buf[INTBUFSIZE];
int *buf_ptr = buf;

while (havedata() && buf_ptr < (buf + sizeof(buf))) {
  *buf_ptr++ = parseint(getdata());
}