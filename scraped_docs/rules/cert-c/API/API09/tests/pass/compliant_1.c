size_t atomicio(ssize_t (*f) (int, void *, size_t),
                int fd, void *_s, size_t n) {
  char *s = _s;
  size_t pos = 0;
  ssize_t res;
  struct pollfd pfd;

  pfd.fd = fd;
  pfd.events = f == read ? POLLIN : POLLOUT;
  while (n > pos) {
    res = (f) (fd, s + pos, n - pos);
    switch (res) {
      case -1:
        if (errno == EINTR)
          continue;
        if (errno == EAGAIN) {
          (void)poll(&pfd, 1, -1);
          continue;
        }
        return 0;
      case 0:
        errno = EPIPE;
        return pos;
      default:
        pos += (size_t)res;
      }
    }
  return (pos);
}