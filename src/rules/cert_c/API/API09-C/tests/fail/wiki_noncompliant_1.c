/*
 * Rule: API09-C
 * Source: wiki
 * Status: FAIL - Should trigger API09-C violation
 */

ssize_t atomicio(f, fd, _s, n)
  ssize_t (*f) (int, void *, size_t);
  int fd;
  void *_s;
  size_t n;
{
  char *s = _s;
  ssize_t res, pos = 0;

  while (n > pos) {
    res = (f) (fd, s + pos, n - pos);
    switch (res) {
      case -1:
         if (errno == EINTR || errno == EAGAIN)
         continue;
      case 0:
        return (res);
      default:
        pos += res;
     }
   }
   return (pos);
}